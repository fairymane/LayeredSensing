
from myo_util import * #TODO: Refactor the myo_util to OO, import class

    

def rt_rec(infile, label_)

    hdf_file = '../data/unit_patterns1.h5'


    real_time_stream(infile, 2, label = label_, get_traning_data = False)

if __name__ =="__main__":

    label_ = -1
    rt_rec(sys.argv[1], label_)
