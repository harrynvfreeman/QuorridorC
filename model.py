from keras.models import Model
from keras.layers import Conv2D, BatchNormalization, Activation, Add, Dense, LeakyReLU, Input, Flatten
from keras.optimizers import SGD
from keras import regularizers
import tensorflow as tf
from loss import softmax_cross_entropy_with_logits

#To change model learning rate
#from keras import backend as K
#K.set_value(model.optimizer.lr, 0.001)
#K.get_value(test.optimizer.lr)

regConstant = 0.0001
learningRate = 0.1
momentum = 0.9
inputShape = (9,9,65)
outputShape = 12 + 2*64

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
  y = Dense(outputShape, name = 'policyHead', kernel_regularizer = regularizers.l2(regConstant), bias_initializer='random_uniform')(d)
  return y

def buildNetwork(x, numRes = 19):
  a = convLayer(x);
  for i in range(numRes):
    a = residualLayer(a)
  
  value = valueHead(a)
  policy = policyHead(a)
  return value, policy

def buildModel():
  a = Input(shape=inputShape)
  value, policy = buildNetwork(a)
  
  losses = {
    "valueHead": "mean_squared_error",
    "policyHead": softmax_cross_entropy_with_logits,
  }
  lossWeights = {"valueHead": 1.0, "policyHead": 1.0}

  model = Model(inputs = [a], outputs = [value, policy])
  model.compile(optimizer=SGD(lr=learningRate, momentum = momentum, clipvalue=1), loss=losses, loss_weights=lossWeights)
  return model