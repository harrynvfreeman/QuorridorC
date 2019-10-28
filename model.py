from keras.models import Model
from keras.layers import Conv2D, BatchNormalization, Activation, Add, Dense, LeakyReLU, Input, Flatten
from keras.optimizers import SGD
from keras import regularizers
import tensorflow as tf
from loss import softmax_cross_entropy_with_logits

#To change model learning rate
#from keras import backend as K
#K.set_value(model.optimizer.lr, 0.001)
#K.get_value(model.optimizer.lr)

regConstant = 0.0001
learningRate = 0.001
momentum = 0.9
#momentum = 1
inputShape = (17,17,29)
outputShape = 140

def convLayer(x):
  a = Conv2D(256, [3, 3], padding='same', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(x)
  b = BatchNormalization()(a)
  y = LeakyReLU()(b)
  return y

def residualLayer(x):
  a = Conv2D(256, [3, 3], padding='same', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(x)
  b = BatchNormalization()(a)
  c = LeakyReLU()(b)
  d = Conv2D(256, [3, 3], padding='same', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(c)
  e = BatchNormalization()(d)
  f = Add()([e, x])
  y = LeakyReLU()(f)
  return y

def valueHead(x):
  a = Conv2D(1, [1, 1], padding='same', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(x)
  b = BatchNormalization()(a)
  c = LeakyReLU()(b)
  d = Flatten()(c)
  e = Dense(256, kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(d)
  f = LeakyReLU()(e)
  y = Dense(1, activation='tanh', name = 'valueHead', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(f)
  return y

def policyHead(x):
  a = Conv2D(2, [1, 1], padding='same', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(x)
  b = BatchNormalization()(a)
  c = LeakyReLU()(b)
  d = Flatten()(c)
  y0 = Dense(2, name = 'policyTypeHead', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(d)
  y1 = Dense(12, name = 'policyMoveHead', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(d)
  y2 = Dense(outputShape-12, name = 'policyBlockHead', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(d)
  return y0, y1, y2

def buildNetwork(x, numRes = 10):
  a = convLayer(x);
  for i in range(numRes):
    a = residualLayer(a)
  
  value = valueHead(a)
  policy_move_type, policy_move, policy_block = policyHead(a)
  return value, policy_move_type, policy_move, policy_block

def buildModel():
  a = Input(shape=inputShape)
  value, policy_move_type, policy_move, policy_block = buildNetwork(a)
  
  losses = {
    "valueHead": "mean_squared_error",
    "policyTypeHead": softmax_cross_entropy_with_logits,
    "policyMoveHead": softmax_cross_entropy_with_logits,
    "policyBlockHead": softmax_cross_entropy_with_logits,
  }
  lossWeights = {"valueHead": 1, "policyTypeHead": 1, "policyMoveHead": 1, "policyBlockHead": 1}

  model = Model(inputs = [a], outputs = [value, policy_move_type, policy_move, policy_block])
  model.compile(optimizer=SGD(lr=learningRate, momentum = momentum), loss=losses, loss_weights=lossWeights)
  return model