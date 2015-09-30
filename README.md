# ![LayeredSensing](Paper_Slides/myo_image_black.png)

Using motion sensing and muscle electrical signals to detect user's daily activiy in every second granularity.
A plugin applcation to social networks or other mobile Apps to enhance user experice by high accuracy of activity recognition.

## Install
### Data Visaulization

```
 Dependencies: ipython notebook, pandas, plotly.
```

```
$ pip install pandas
$ pip install ipython
$ pip install "ipython[notebook]"
```
Or you can install Anaconda, full Python environment for scientific computing and data science, which includes above packages
1. Download and install Continuum’s [Anaconda](https://www.continuum.io/downloads).
2. Update IPython to the current version using the Terminal:

```
$ conda update conda
$ conda update ipython ipython-notebook ipython-qtconsole
```

```
$ pip install plotly
```
you need to setup an account of plotly in order to run the python code:
    https://plot.ly/python/streaming-tutorial/
    https://plot.ly/python/getting-started/

### Right now all the changes were made in sdk/samples/hello-myo.cpp, which would store all the IMU and EMG data into 2 files, imu_data.csv and emg_data.csv in sdk/output.

### You can run the imu_stream_disp.py and emg_stream_disp.py in sdk/samples/python/ to visualize the data stream
