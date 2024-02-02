import matplotlib.pyplot as plt

# with open("saved_files/frequency_spectrum_12.csv", "r") as f:
file_name = "saved_files/G2.csv"
with open(file_name, "r") as f:

    lines = f.readlines()[1:]
    pairs = [l.split(',') for l in lines]

    freq = [float(p[0]) for p in pairs]
    mag = [float(p[1]) for p in pairs]

    clamp_lower = 70
    clamp_higher = 1200

    clamp_lower_i = 0
    clamp_higher_j = 0

    for e,i in enumerate(freq):
        if i > clamp_lower:
            clamp_lower_i = e
            break

    for e,i in enumerate(freq):
        if i > clamp_higher:
            clamp_higher_j = e
            break

    print(clamp_lower_i, clamp_higher_j)

    freq = freq[clamp_lower_i:clamp_higher_j]
    mag = mag[clamp_lower_i:clamp_higher_j]

    # display labels on points
    for i in range(len(freq)):
        if mag[i] > 5:
            plt.text(freq[i], mag[i], str(freq[i]))


    plt.bar(freq, mag)
    plt.title(f"Frequency Spectrum for {file_name}")
    # plt.yscale('log')
    plt.xticks(range(0, 1000, 50), rotation=70)
    plt.xlim(0, 1200)
    # plt.grid()
    plt.show()
