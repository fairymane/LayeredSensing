print(__doc__)

# Author: Ron Weiss <ronweiss@gmail.com>, Gael Varoquaux
# License: BSD 3 clause

# $Id$

import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import pandas as pd

from sklearn import datasets
from sklearn.cross_validation import StratifiedKFold
from sklearn.externals.six.moves import xrange
from sklearn.mixture import GMM
from scipy.stats import multivariate_normal
import scipy.stats
import cmath

import pickle


def make_ellipses(gmm, ax):
    for n, color in enumerate('rgb'):
        v, w = np.linalg.eigh(gmm._get_covars()[n][:2, :2])
        u = w[0] / np.linalg.norm(w[0])
        angle = np.arctan2(u[1], u[0])
        angle = 180 * angle / np.pi  # convert to degrees
        v *= 9
        ell = mpl.patches.Ellipse(gmm.means_[n, :2], v[0], v[1],
                                  180 + angle, color=color)
        ell.set_clip_box(ax.bbox)
        ell.set_alpha(0.5)
        ax.add_artist(ell)

np.random.seed(42)

hdf = pd.HDFStore('unit_patterns_zhiyuan.h5')
df= hdf['df_r_l2']
df1 = hdf['df_tooth']
df2 = hdf['df_cut']
df3 = hdf['df_walking2']
df4 = hdf['df_small_l1']
df5 = hdf['df_small_walking2']
df6 = hdf['df_small_cut']
df7 = hdf['df_small_tooth']
hdf.close()

labels_ = df['label_index'].values.astype(int)
data_ = df.ix[:, 1:-1].values
data1_ = df1.ix[:, 1:].values
data2_ = df2.ix[:, 1:].values
data3_ = df3.ix[:, 1:].values
data4_ = df4.ix[:, 1:].values
data5_ = df5.ix[:, 1:].values
data6_ = df6.ix[:, 1:].values
data7_ = df7.ix[:, 1:].values
print data3_.shape

# Break up the dataset into non-overlapping training (75%) and testing
# (25%) sets.
skf = StratifiedKFold(labels_, n_folds=4)
# Only take the first fold.
train_index, test_index = next(iter(skf))


#X_train = data_[train_index]
#y_train = labels_[train_index]
#X_test = data_[test_index]
#y_test = labels_[test_index]

X_train = data4_[train_index]
y_train = labels_[train_index]
X_test = data4_[test_index]
y_test = labels_[test_index]

n_classes = len(np.unique(y_train))
#print 'np.unique(y_train): ', np.unique(y_train)
class_list = np.unique(y_train)


#n_classes = 5

# Try GMMs using different types of covariances.
classifiers = dict((covar_type, GMM(n_components=n_classes,
                    covariance_type=covar_type, init_params='wc', n_iter=60))
                   for covar_type in ['spherical', 'diag', 'tied', 'full'])

n_classifiers = len(classifiers)

plt.figure(figsize=(3 * n_classifiers / 2, 6))
plt.subplots_adjust(bottom=.01, top=0.95, hspace=.15, wspace=.05,
                    left=.01, right=.99)

#print 'n_classes: ', n_classes
#print 'classifiers: \n',  classifiers

for index, (name, classifier) in enumerate(classifiers.items()):
    # Since we have class labels for the training data, we can
    # initialize the GMM parameters in a supervised manner.

    print 'index: ', index
    print 'name: ', name

    classifier.means_ = np.array([X_train[y_train == i].mean(axis=0)
                                  for i in class_list ])

    # Train the other parameters using the EM algorithm.
    #print 'X_train: \n',X_train
    print X_train.shape
    classifier.fit(X_train)

    h = plt.subplot(2, n_classifiers / 2, index + 1)
    make_ellipses(classifier, h)

    for n, color in enumerate('rgbyk'):
        #print 'n: ', n
        #print 'labels: ', labels_ == (n + 1) 
        data = data_[labels_ == (n + 1) ]

        plt.scatter(data[:, 0], data[:, 1], 0.8, color=color)

    # Plot the test data with crosses
    for n, color in enumerate('rgbyk'):
        data = X_test[y_test == (n + 1) ]
        plt.plot(data[:, 0], data[:, 1], 'x', color=color)

    classifiers[name] = classifier    

    y_train_pred = classifier.predict(X_train)

    y_train_pred_prob =  classifier.predict_proba(X_train)

    y_label_train_prob = np.argmax(y_train_pred_prob, axis=1) + 1

    print 'y_label_train_prob: ', y_train.ravel()
    print 'classifiers covariances: ',classifier.covars_

    #y_train_pred = [i + 1 for i in y_train_pred]
    y_train_pred += 1
    #print 'orig train labels: ', y_train
    #print 'estimated train labels: ', y_train_pred
    #print 'y_label_training_probability: ', y_train_pred_prob

    train_accuracy = np.mean(y_train_pred.ravel() == y_train.ravel()) * 100

    train_accuracy2 = np.mean(y_label_train_prob.ravel() == y_train.ravel()) * 100


    #print "y_train_pred_prob: \n", y_train_pred_prob.sum(1)

    print 'train_accuracy: ', train_accuracy

    print 'train_accuracy222222: ', train_accuracy2
    plt.text(0.05, 0.9, 'Train accuracy: %.1f' % train_accuracy,
             transform=h.transAxes)

    [t1,t2] = X_test.shape
    [t11,t12] = data7_.shape
    trained_means = classifier.means_
    trained_covars = classifier.covars_
    
    if (name == 'full'):
        var_dist_list = []
        for i in xrange(5):
            var_dist = multivariate_normal(trained_means[i],trained_covars[i])
            var_dist_list.append(var_dist)

        res = np.zeros( (t1, 5) )   
        res1 = np.zeros((t11,5)) 
        for i in xrange(t1):
            for j in xrange(5) :
            #print scipy.stats.norm(loc = trained_means[0], scale = np.sqrt(trained_covars[0])).cdf(X_test[i])
                res[i, j] = var_dist_list[j].pdf(X_test[i])
            #print 'res: \n', res
        for i in xrange(t11):
            for j in xrange(5) :
            #print scipy.stats.norm(loc = trained_means[0], scale = np.sqrt(trained_covars[0])).cdf(X_test[i])
                res1[i, j] = var_dist_list[j].pdf(data7_[i])
        test_pred = np.argmax(res, axis = 1) + 1
        maximum_v = np.amax(res, axis = 1)
        maximum_v1 = np.amax(res1, axis = 1)
        print 'Maximum Value of Each Row: ',maximum_v1
        print 'Above Threshold index: \n', float(np.asarray(np.where(maximum_v1>1e-75)).size)/float(t11)
        print 'Above Threshold index1: \n',float(np.asarray(np.where(maximum_v>1e-75)).size)/float(t1)
        print 'Calculated predictions: \n', test_pred
    
            #print p  
    y_test_pred = classifier.predict(X_test)
    y_test_pred_prob =  classifier.predict_proba(X_test)

    y_label_test_prob = np.argmax(y_test_pred_prob, axis=1) + 1

    #y_test_pred = [i + 1 for i in y_test_pred]
    y_test_pred += 1
    #print 'orig test labels: ', y_test
    print 'estimated test labels: \n', y_test_pred
    test_accuracy = np.mean(y_test_pred.ravel() == y_test.ravel()) * 100


    #print 'y_label_test_prob: ', y_label_test_prob
    test_accuracy2 = np.mean(y_label_test_prob.ravel() == y_test.ravel()) * 100

    #print "y_test_pred_prob: \n", y_test_pred_prob.sum(1)

    print 'test accuracy: ', test_accuracy

    print 'test accuracy 222222: ', test_accuracy2
    plt.text(0.05, 0.8, 'Test accuracy: %.1f' % test_accuracy,
             transform=h.transAxes)

    plt.xticks(())
    plt.yticks(())
    plt.title(name)


with open('gmm_models.pickle', 'wb') as f:
    pickle.dump(classifiers, f)

plt.legend(loc='lower right', prop=dict(size=12))


plt.show()