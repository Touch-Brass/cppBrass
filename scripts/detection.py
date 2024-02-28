import serial
import numpy as np
import matplotlib.pyplot as plt

# Define the COM port and baud rate
port = 'COM8'  # Change this to the appropriate COM port
baud_rate = 2000000

# Number of samples to collect before performing FFT
samples_per_fft = 1000

try:
    # Open the serial port
    ser = serial.Serial(port, baud_rate)

    # Initialize lists to store samples
    samples = []

    # Turn on interactive mode
    plt.ion()

    while True:
        # Read a line of data from the serial port
        data = ser.readline().decode('utf-8').strip()

        if data == "":
            print(".", end="")
            continue

        # Convert data to integer (assuming it's numeric)
        sample = int(data)
        samples.append(sample)

        # If we have collected enough samples, perform FFT
        if len(samples) == samples_per_fft:
            # Perform FFT
            fft_result = np.abs(np.fft.fft(samples))
            freq = np.fft.fftfreq(len(samples), d=1 / 1000)

            middle = len(fft_result) // 2

            fft_result = fft_result[10:middle - 10]
            freq = freq[10:middle - 10] / 1.265

            # Plot FFT result
            plt.clf()  # Clear previous plot
            plt.stem(freq, fft_result, markerfmt=" ")
            plt.ylim([0, 80000])
            plt.title('FFT Result')
            plt.xlabel('Frequency')
            plt.ylabel('Magnitude')

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
