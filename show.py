#!/usr/bin/env python3
from tkinter import Tk, Entry, TOP, BOTTOM, BOTH
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

window = Tk()
window.wm_title("displaying event 0/125 in run 0")

canvas = FigureCanvasTkAgg(fig, master=window)  # A tk.DrawingArea.
canvas.draw()
canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=1)

def run_cmd():
    cmdline.get()

cmdline=Entry(window)
cmdline.pack(side=BOTTOM, fill=BOTH)
cmdline.bind('<Escape>', canvas.get_tk_widget().focus_set())
cmdline.bind('<Return>', run_cmd())
 
def handle_key_press(event):
    print("you pressed {}".format(event.key))
    if format(event.key) == "q":
        window.quit()    # stop mainloop
        window.destroy() # necessary on Windows to prevent fatal error
    elif format(event.key) == "escape":
        cmdline.clear()
        canvas.get_tk_widget().focus_set()
    if format(event.key)=="g":
        cmdline.focus_set()
    else:
        key_press_handler(event, canvas)

canvas.mpl_connect("key_press_event", handle_key_press)

# If you put window.destroy() here, it will cause an error if the window is
# closed with the window manager.

# https://stackoverflow.com/questions/17774859
from os import system
from platform import system as platform

if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

window.mainloop()