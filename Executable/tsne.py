import sys
import os
from time import time
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.ticker import NullFormatter
from sklearn import manifold, datasets
import numpy as np
from sklearn.decomposition import PCA


dimension=2

input_size=int(sys.argv[1])
feature_size=int(sys.argv[2])

X=np.fromfile('tsneInput.raw',dtype=np.float32)
np.savetxt('check.txt',X)
#X=X.byteswap().newbyteorder()
X=np.reshape(X,(input_size,feature_size))


t0 = time()
tsne = manifold.TSNE(angle=0.1, early_exaggeration=10.0, init='pca', learning_rate=100.0,
                     method='barnes_hut', metric='euclidean', min_grad_norm=1e-07,
                     n_components=dimension, n_iter=100000, n_iter_without_progress=300,
                     perplexity=5.0, random_state=0, verbose=0)
print(tsne)
Y = tsne.fit_transform(X)



#pca = PCA(n_components=dimension)

#print(pca)
#Y = pca.fit_transform(X)




t1 = time()
print("t-SNE: %.2g sec" % (t1 - t0))
np.savetxt('check3.txt',Y)

#Y2=np.zeros(input_size*2)
#for i in range(input_size):
#    Y2[i*2+0]=Y[i]
#    Y2[i*2+1]=Y[i]
#
#Y=Y2

Y=np.reshape(Y,(input_size*2))
Y=Y.astype(np.float32)

np.savetxt('check2.txt',Y)

#file=open('tsneResult.raw','wb')
#file.write(Y.tobytes())
#file.close()
#Y=Y.byteswap().newbyteorder()
Y.tofile('tsneResult.raw')
