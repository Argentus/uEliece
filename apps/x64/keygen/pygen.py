#!/usr/bin/env python

import numpy as np
import scipy.sparse as sps

X = 9857
w0 = 71


uarray = np.random.choice(X, replace=False, size=(1, w0))

sm = sps.lil_matrix((X, X), dtype=bool)

for row in range(X):
    for j, i in enumerate(uarray[0]):
        sm[row, i] = True
        uarray[0][j] += 1
        if uarray[0][j] > X-1:
            uarray[0][j] = 0

uarray2 = np.random.choice(X, replace=False, size=(1, w0))

sm2 = sps.lil_matrix((X, X), dtype=bool)

for row in range(X):
    for j, i in enumerate(uarray[0]):
        sm2[row, i] = True
        uarray2[0][j] += 1
        if uarray2[0][j] > X-1:
            uarray2[0][j] = 0

m = sps.csr_matrix(sm2)
np.linalg.pinv(m.toarray())
