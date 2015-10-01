#!/usr/bin/python
from myo_util import * #TODO: Refactor the myo_util to OO, import class

    
    label_index = {'idle_sitting':4, 'dribbling': 3, 'walking': 2, 'shooting': 1}
    index_label = {v: k for k, v in label_index.items()}

    act_label_index = {'LIVE_CONCERT' : 1, 'PLAY_BASKETBALL' :2, 'GUITA_PRACTICE' : 3}
    act_index_label = {v: k for k, v in act_label_index.items()}

    hdf_file = '../data/unit_patterns1.h5'
    global pca_
    global model_
    global act_model_
    global act_pca_
    global cluster_model_


    label_ = -1
    if len(sys.argv) > 3:
        label_ =  sys.argv[3]


def define(sys.argv])
    real_time_stream(sys.argv[1], 2, label = label_, get_traning_data = True)
