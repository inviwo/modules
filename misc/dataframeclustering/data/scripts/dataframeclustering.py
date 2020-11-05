
import numpy as np
import math

from sklearn.preprocessing import RobustScaler

"""
The following variables `dataframe`, `activeColumnheaders`, `method`, and `numClusters`
as well as `linkage`, `N`, `e` must be provided by the python environment.

See also dataframeclustering/processors/dataframeclustering.cpp

"""


def remapLables(labels):
    """
    Changes the label indices based on the size of each cluster, such that the 
    cluster with most members the lowest index (zero), and the cluster with the 
    least members gets the hightest index.
    Negative label indices are left unchanged. (used, for example, to indicate noise in DBSCAN)
    """
    res = np.copy(labels)
    label, count = np.unique(labels,return_counts=True)
    freq = list(zip(label,count))
    firstPostive = np.argmax(label > -1)
    
    for i,(l,f) in enumerate(sorted(freq[firstPostive:], key=lambda x: x[1], reverse=True)):
        res[labels == l] = i
    return res
        

df = np.zeros([dataframe.rows,len(activeColumnHeaders)])

useStd = True

idx = 0
for i in range(dataframe.cols):
    col = dataframe[i]
    if col.header in activeColumnHeaders:
        df[:,idx] = col.buffer.data
        colmin = np.amin(df[:,idx])
        colmax = np.amax(df[:,idx])
        if not math.isfinite(colmin) or not math.isfinite(colmax):
            raise ValueError("Column " + col.header + " has nan/inf values")
        if colmax != colmin:
            # Scale features using statistics that are robust to outliers.
            df[:,idx] = RobustScaler().fit_transform(df[:,idx].reshape(-1,1))[:,0]

        idx = idx + 1
        

if method == "dbscan":
    from sklearn.cluster import DBSCAN
    clusters = DBSCAN(eps=e, min_samples=N).fit(df)
    labels = clusters.labels_.astype('int32')
    
elif method == "spectral":
    from sklearn.cluster import SpectralClustering
    clusters = SpectralClustering(n_clusters=numClusters, eigen_solver='arpack').fit(df)
    labels = clusters.labels_.astype('int32')

elif method == "agglo":
    from scipy.cluster.hierarchy import linkage as L
    from scipy.cluster.hierarchy import cut_tree as Cut

    tree = L(df,method=linkage)
    cut = Cut(tree,n_clusters=[numClusters]) 
    labels = cut.reshape(cut.shape[0]) 

else:
    from sklearn.cluster import KMeans
    k_means = KMeans(init='k-means++', n_clusters=numClusters, n_init=10)
    k_means.fit(df)
    labels = k_means.labels_

numberOfFoundClusters = np.unique(labels).size

labels = remapLables(labels)     