import sys
import os
import numpy as np
import umap

dimension=2

input_size=int(sys.argv[1])
feature_size=int(sys.argv[2])

X=np.fromfile('tsneInput.raw',dtype=np.float32)
np.savetxt('check.txt',X)
X=np.reshape(X,(input_size,feature_size))


fit = umap.UMAP(
    n_neighbors=10,
    min_dist=0.1,
    n_components=2,
    metric='euclidean',
    random_state=1
)
Y = fit.fit_transform(X)

Y=np.reshape(Y,(input_size*2))
Y=Y.astype(np.float32)

Y.tofile('tsneResult.raw')
