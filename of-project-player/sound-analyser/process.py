import matplotlib.pyplot as plt
import json
from numpy.fft import fft
from scipy.io import wavfile  # get the api
import matplotlib.animation as animation
import numpy as np
import subprocess
from scipy.signal.windows import hamming
import math
import sys
import argparse


"""
Copying and distribution of this file, with or without modification, are 
permitted in any medium provided you do not contact the author about the file or 
any problems you are having with the file.
"""

filename = ""

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="process.py",
        description="Use FFT to make pattern for lights",
        epilog="send help",
    )

    parser.add_argument("filename")
    parser.add_argument("-p", "--plot", action="store_true")
    args = parser.parse_args()
    filename = args.filename


sample_rate, data = wavfile.read(filename)  # load the data
a = data.T[0]  # this is a two channel soundtrack, I get the first track
i = 0


fps = 1 / 10
samples = int(fps * sample_rate)

num_bins = 30
result = {}


class loader:
    def binerise(self, data: list, nbins: int):
        res = []
        binsize = int(math.floor(len(data) / nbins))
        for i in range(nbins):
            res.append(max(data[i * binsize : (i + 1) * binsize]))
        for i in range(len(res)):
            res[i] = int(res[i] * 3)
            if res[i] > 155:
                res[i] = int(255)

        return res

    def load(self):
        frame_number = 0
        while (frame_number + 1) * samples < len(a):
            timenow = fps * frame_number
            # simulate new data coming in
            b = [
                (a[ele] / 2**16.0) * 2 - 1
                for ele in range(frame_number * samples, (frame_number + 1) * samples)
            ]  # this is 8-bit track, b is now normalized on [-1,1)
            w = hamming(len(b))
            c = fft(b)  # calculate fourier transform (complex numbers list)

            pl = abs(c)  # [: int(len(c)) ]
            w = np.linspace(1, 44000, len(pl))
            result[timenow * 1000] = self.binerise((np.abs(pl[1:500])), num_bins)
            frame_number += 1
        return result

    def prepare_animation(plot):
        loader().load()

        def animate(frame_number):
            timenow = 1000 * (fps * frame_number)
            plot.set_ydata(result[timenow])

        return animate


l = loader()
result = l.load()
with open(filename.replace(".wav", ".json"), "w") as f:
    # two arrays since json doesnt allow sotring maps with integer keys
    # and we want to represent a sparse array
    timestamps = list(result.keys())
    f.write(
        json.dumps(
            {
                "filename": filename,
                "numbins": num_bins,
                "times": timestamps,
                "frames": [result[i] for i in timestamps],
            }
        )
    )


if args.plot:
    fig, ax = plt.subplots()

    (plot,) = ax.plot([0 for i in range(num_bins - 1)] + [255])
    ani = animation.FuncAnimation(
        fig, loader.prepare_animation(plot), 50000, interval=1000 * fps
    )
    subprocess.Popen(f'aplay "{filename}"', shell=True)

    plt.show()
