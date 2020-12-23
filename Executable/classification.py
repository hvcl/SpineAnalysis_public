import sys
import os
from time import time
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.ticker import NullFormatter
from sklearn import manifold, datasets
import numpy as np
from sklearn.neural_network import MLPClassifier
from sklearn.ensemble import RandomForestClassifier
import pickle

input_size=int(sys.argv[1])
feature_size=int(sys.argv[2])
label_num=int(sys.argv[3])


#with open('setting/defaultSize.txt') as f:
#    dinput_size, dfeature_size, dlabel_num = [int(x) for x in next(f).split()] # read first line


#dX=np.fromfile('setting/defaultInput.raw',dtype=np.float32)
#dY=np.fromfile('setting/defaultLabel.raw',dtype=np.int32)


X=np.fromfile('classificationInput.raw',dtype=np.float32)
Y=np.fromfile('classificationLabel.raw',dtype=np.int32)

#input_size+=dinput_size

#X=np.append(dX,X)
#Y=np.append(dY,Y)

X=np.reshape(X,(input_size,feature_size))

rfc=RandomForestClassifier(n_estimators=30, oob_score=True,
                               max_features="sqrt")

#with open('model.pkl', 'rb') as fid:
#    rfc=pickle.load(fid)
	
rfc.fit(X,Y)

with open('model.pkl', 'wb') as fid:
    pickle.dump(rfc, fid)
    
print('***oob_score: %s'% rfc.oob_score_)

data=np.fromfile('classificationData.raw',dtype=np.float32)
data=np.reshape(data,(int(data.size/feature_size),feature_size))

res=rfc.predict_proba(data)
res=res.astype(np.float32)
res.tofile('classificationResult.raw')
