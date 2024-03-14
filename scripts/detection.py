import serial
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import time

# Define the COM port and baud rate
port = '/dev/tty.usbmodem2101'  # Change this to the appropriate COM port
# macOS: plug in arduino w/ signal handler and use ls /dev/tty.* to figure out port in use: e.g. '/dev/tty.usbmodem2101'
baud_rate = 115200
rate  = 2100
timestep = 1./rate

# Number of samples to collect before performing FFT
samples_per_fft = 1000

try:
    # Open the serial port
    ser = serial.Serial(port, baud_rate)

    # Initialize lists to store samples
    samples = []

    # Turn on interactive mode
    plt.ion()

    start_time = time.time()
    data_count = 0
    interval = 5

    while True:
        # Read a line of data from the serial port
        data = ser.readline().decode('utf-8').strip()

        if data == "":
            print(".", end="")
            continue

        # Convert data to integer (assuming it's numeric)
        sample = int(data)
        samples.append(sample)

        # Count the number of loops to determine actual sample rate
        data_count += 1
        current_time = time.time()
        elapsed_time = current_time - start_time
        if elapsed_time > interval:
            rate = data_count/elapsed_time
            timestep = 1./rate
            # print(f"Rate: {rate} bytes/second")
            start_time = current_time
            data_count = 0

        # If we have collected enough samples, perform FFT
        if len(samples) == samples_per_fft:
            # Perform FFT
            fft_result = np.abs(np.fft.rfft(samples))
            freq = np.fft.rfftfreq(len(samples), d=timestep)


            # Spectrum Clean-up
            # Removing DC component.
            fft_result = fft_result[1:]
            freq = freq[1:]

            # Removing frequencies below 82.4 Hz (lowest note for trombone)
            cutoff = 82.4
            index = np.where(freq>cutoff)[0][0]
            fft_result[:index] = 0

            
            # Determine Fundamental Frequency using HPS technique
            hps_spectrum = fft_result.copy()
            for i in [2,3]:
                decimated_signal = signal.decimate(samples, i)
                decimated_spectrum = np.abs(np.fft.rfft(decimated_signal))
                decimated_fft_result = np.ones(hps_spectrum.shape)
                decimated_fft_result[:len(decimated_spectrum)] = decimated_spectrum
                hps_spectrum = hps_spectrum*decimated_fft_result
            hps_spectrum = np.log10(hps_spectrum)
            
            peaks, peak_amps = signal.find_peaks(hps_spectrum, 10)

            highest_index = 0
            highest_amp = 0
            height_index = 0
            for amp in peak_amps["peak_heights"]:
                if amp > highest_amp:
                    highest_index = height_index
                    highest_amp = amp
                height_index += 1
            
            if(len(peaks)>0):
                peak = peaks[highest_index]
                fundamental_freq = freq[peak]
                print(fundamental_freq)

            # Current mismatch by factor of about 1.17~1.19
            # 261.63 read 222
            # 392 read 333
            # 440 read 369

            # Plot FFT result
            plt.clf()  # Clear previous plot
            plt.subplot(2,1,1)
            plt.stem(freq, fft_result, markerfmt=" ")
            plt.ylim([0, 80000])
            plt.title('FFT Result')
            plt.xlabel('Frequency')
            plt.ylabel('Magnitude')
            plt.subplot(2,1,2)
            plt.stem(freq, hps_spectrum, markerfmt=" ")

            # Find index of maximum peak
            max_peak_indices = np.argsort(fft_result)[-3:][::-1]

            for idx in max_peak_indices:
                max_peak_freq = freq[idx]
                max_peak_magnitude = fft_result[idx]

                # Display maximum peak value on the graph
                plt.annotate(f'{max_peak_freq:.2f}', (max_peak_freq, min(max_peak_magnitude, 77000)))


            plt.draw()
            plt.pause(0.001)  # Pause to update the plot

            # Clear the list for the next set of samples
            samples = []

            # input("Press Enter to continue...")
            ser.reset_input_buffer()
            # ser.flush()
    # print(np.array(samples))
    # input("Press Enter to continue...")
    # scipy.io.wavfile.write("audio.wav", 1000, np.array(samples))

except serial.SerialException as e:
    print("Error: ", e)
finally:
    # Close the serial port
    ser.close()
