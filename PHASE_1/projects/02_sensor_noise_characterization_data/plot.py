import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from statsmodels.graphics.tsaplots import plot_acf

data_5ms = pd.read_csv("data_5ms_adjusted.csv")
data_20ms = pd.read_csv("data_20ms_adjusted.csv")
data_100ms = pd.read_csv("data_100ms_adjusted.csv")

#calculates the noise amplitud of the data
noise_amp_5ms = data_5ms["adc"].max() - data_5ms["adc"].min()
noise_amp_20ms = data_20ms["adc"].max() - data_20ms["adc"].min()
noise_amp_100ms = data_100ms["adc"].max() - data_100ms["adc"].min()

#frecuency of the noise (using fast fourier transformation)
[t_5ms, y_5ms] = [data_5ms["time_ms"].values, data_5ms["adc"].values]
[t_20ms, y_20ms] = [data_20ms["time_ms"].values, data_20ms["adc"].values]
[t_100ms, y_100ms] = [data_100ms["time_ms"].values, data_100ms["adc"].values]
## set the sampling interval
dt_5ms = t_5ms[1] - t_5ms[0]
dt_20ms = t_20ms[1] - t_20ms[0]
dt_100ms = t_100ms[1] - t_100ms[0]
## FFT
Y_5ms = np.fft.fft(y_5ms)
freqs_5ms = np.fft.fftfreq(len(Y_5ms), dt_5ms)
Y_20ms = np.fft.fft(y_20ms)
freqs_20ms = np.fft.fftfreq(len(Y_20ms), dt_20ms)
Y_100ms = np.fft.fft(y_100ms)
freqs_100ms = np.fft.fftfreq(len(Y_100ms), dt_100ms)
## getting just possitive frecuencies
idx_5ms = np.where(freqs_5ms > 0)
freqs_5ms = freqs_5ms[idx_5ms]
Y_5ms = np.abs(Y_5ms[idx_5ms])
idx_20ms = np.where(freqs_20ms > 0)
freqs_20ms = freqs_20ms[idx_20ms]
Y_20ms = np.abs(Y_20ms[idx_20ms])
idx_100ms = np.where(freqs_100ms > 0)
freqs_100ms = freqs_100ms[idx_100ms]
Y_100ms = np.abs(Y_100ms[idx_100ms])
## Dominant frecuency
dominant_freq_5ms = freqs_5ms[np.argmax(Y_5ms)]
dominant_freq_20ms = freqs_20ms[np.argmax(Y_20ms)]
dominant_freq_100ms = freqs_100ms[np.argmax(Y_100ms)]

# statistical characterization
## mean (offset)
mean_5ms = np.mean(y_5ms)
mean_20ms = np.mean(y_20ms)
mean_100ms = np.mean(y_100ms)
## standard deviation
std_5ms = np.std(y_5ms)
std_20ms = np.std(y_20ms)
std_100ms = np.std(y_100ms)


# Plot ADC signal
fig, axs = plt.subplots(3,4)
line5ms, = axs[0,0].plot(data_5ms["time_ms"], data_5ms["adc"], color='red')
line20ms, = axs[1,0].plot(data_20ms["time_ms"], data_20ms["adc"], color='green')
line100ms, = axs[2,0].plot(data_100ms["time_ms"], data_100ms["adc"], color='blue')
# Plot Histogram
axs[0,1].hist(data_5ms["adc"], color='red')
axs[1,1].hist(data_20ms["adc"], color='green')
axs[2,1].hist(data_100ms["adc"], color='blue')
# Plot FFT frequencies
axs[0,2].plot(freqs_5ms, Y_5ms, color='red')
axs[1,2].plot(freqs_20ms, Y_20ms, color='green')
axs[2,2].plot(freqs_100ms, Y_100ms, color='blue')
## plot autocorrelation
pd.plotting.lag_plot(data_5ms["adc"], lag=1, ax=axs[0,3],s=5)
pd.plotting.lag_plot(data_20ms["adc"], lag=1, ax=axs[1,3],s=5)
pd.plotting.lag_plot(data_100ms["adc"], lag=1, ax=axs[2,3],s=5)

line5ms.set_label(f"5 ms \n Noise amp: {noise_amp_5ms} \n Dominant freq: {1000*dominant_freq_5ms:.3f} mHz \n $\\mu$={mean_5ms:.2f} $\\pm \\sigma$={std_5ms:.2f}")
line20ms.set_label(f"20 ms \n Noise amp: {noise_amp_20ms} \n Dominant freq: {1000*dominant_freq_20ms:.3f} mHz \n $\\mu$={mean_20ms:.2f} $\\pm \\sigma$={std_20ms:.2f}")
line100ms.set_label(f"100 ms \n Noise amp: {noise_amp_100ms} \n Dominant freq: {1000*dominant_freq_100ms:.3f} mHz \n $\\mu$={mean_100ms:.2f} $\\pm \\sigma$={std_100ms:.2f}")

fig.suptitle("ADC Noise Signal")
axs[0,0].title.set_text('ADC signal')
axs[0,1].title.set_text('ADC histogram')
axs[0,2].title.set_text('FFT Freq')
axs[0,3].title.set_text('Autocorrelation')
fig.legend()

plt.show()