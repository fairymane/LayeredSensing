# ![LayeredSensing](Paper_Slides/myo_image_black.png)

_Using motion sensing and muscle electrical signals to detect user's daily activiy in every second granularity.
A plugin applcation to social networks or other mobile Apps to enhance user experice by high accuracy of activity recognition_.

## Install
#### Data Visaulization

 *Dependencies: ipython notebook, pandas, plotly.*

```
$ pip install pandas
$ pip install ipython
$ pip install "ipython[notebook]"
```
Or you can install Anaconda, full Python environment for scientific computing and data science, which includes above packages.
 1. Download and install Continuum’s [Anaconda](https://www.continuum.io/downloads).
 2. Update IPython to the current version using the Terminal:

```
$ conda update conda
$ conda update ipython ipython-notebook ipython-qtconsole
```

```
$ pip install plotly
$ python -c "import plotly; plotly.tools.set_credentials_file(username='DemoAccount', api_key='lr1c37zw81')"
```
You'll need to setup an account of plotly in order to run the python code.
    * [tutorial](https://plot.ly/python/streaming-tutorial/).
    * [plotly getting started](https://plot.ly/python/getting-started/).

After setup, you need to replace 'DemoAccount' and 'lr1c37zw81' with your Plotly username and [API key](https://plot.ly/settings/api/).


###### MYO API
src/myo_project_data_stream_generation/myo_sensor_data.cpp will generate all the IMU data (motion activities, 9 data streams) and EMG data (muscle activities, 8 data streams) into 2 files, imu_data.csv and emg_data.csv in src/data/.

###### Realtime Visualization
You can run the imu_stream_disp.py and emg_stream_disp.py in src/myo_project_data_stream_generation/ to visualize the data stream in real time while the above program is generating data. For example:

```
$ python imu_stream_disp.py ../data/imu_data.csv
```


