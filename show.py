#!/usr/bin/env python3
import tkinter
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
# handle default matplotlib key bindings
from matplotlib.backend_bases import key_press_handler
from matplotlib.figure import Figure
import uproot4 as uproot

file = uproot.open('run/0/wave0.root')
t = file['t']
b = t.arrays()

fig = Figure(figsize=(5, 4), dpi=100)
fig.add_subplot(111).plot(b[b's'][0])

root = tkinter.Tk()
root.wm_title("Embedding in Tk")

canvas = FigureCanvasTkAgg(fig, master=root)  # A tk.DrawingArea.
canvas.draw()
canvas.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

def _quit():
    root.quit()     # stops mainloop
    root.destroy()  # this is necessary on Windows to prevent
                    # Fatal Python Error: PyEval_RestoreThread: NULL tstate

def on_key_press(event):
    print("you pressed {}".format(event.key))
    key_press_handler(event, canvas)
    if format(event.key) == "q" or format(event.key) == "escape":
        _quit()

canvas.mpl_connect("key_press_event", on_key_press)

# If you put root.destroy() here, it will cause an error if the window is
# closed with the window manager.

# https://stackoverflow.com/questions/17774859
from os import system
from platform import system as platform

if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

root.mainloop()
