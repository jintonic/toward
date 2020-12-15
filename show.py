#!/usr/bin/env python

usage='''
Show waveforms in run/number/wave[0-7].root:
python3 show.py <number>

where <number> should be a non-negative integer, e.g. 123
wave[0-7].root will then be searched in folder run/123/

If no <number> is specified, all sub-directories in run/ 
will be checked if they are named as non-negative integers.
wave[0-7].root will then be searched in run/<biggest integer>
'''

hotkeys='''
   --- List of key bindings ---
0-7:        toggle channel 0 to 7
<Space>:    next event
b:          previous event
q:          quit
'''

# get the largest run in run/*/
runs=[]
from os import scandir, path
with scandir('run') as dirs:
    for folder in dirs:
        if folder.name.isdigit(): runs.append(folder.name)
runs.sort(); run=runs[-1]

# get run number
from sys import argv
if len(argv)>1: # if run number is specified
    if argv[1] in runs: run=argv[1]
    else: print("no run/"+argv[1]+"/, quit"); exit()

# check root files
folder="run/"+run+"/"
print("check ROOT files in "+folder+":")
import uproot4
t=[0]*8; n=[0]*8; nfiles=0
for ch in range(8):
    file=folder+"wave"+str(ch)+".root"
    if path.exists(file):
        t[ch]=uproot4.open(file)['t'].arrays()
        n[ch]=len(t[ch])
        print(file+" contains "+str(n[ch])+" events")
        if n[ch]>0: nfiles+=1
if nfiles<1: print("no root file in "+folder+", quit"); exit()
    
    
# title bar
for ch in range(8):
    if n[ch]>0:
        title="There are "+str(n[ch])+" events in run "+run
        title+=" (press h for help, q to quit)"
        break
from tkinter import Tk, Label, Entry, END, TOP, LEFT, BOTH
window = Tk(); window.wm_title(title)

# canvas
from matplotlib.figure import Figure
fig=Figure(); ax=fig.add_subplot()
evt=0; # draw the first event
channel=[0]*8
for ch in range(8):
    if n[ch]>0:
        line,=ax.plot(t[ch][b's'][evt], label="channel "+str(ch))
        channel[ch]=line
ax.legend()

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
canvas = FigureCanvasTkAgg(fig, master=window); canvas.draw()
canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=1)

# tool bar
from matplotlib.backends.backend_tkagg import NavigationToolbar2Tk
toolbar=NavigationToolbar2Tk(canvas, window)

Label(toolbar, text="Event:").pack(side=LEFT)
evtSpecifier=Entry(toolbar, width=8)
evtSpecifier.insert(0, str(evt))
evtSpecifier.pack(side=LEFT)
Label(toolbar, text="Run:").pack(side=LEFT)
runSpecifier=Entry(toolbar, width=3)
runSpecifier.insert(0, run)
runSpecifier.pack(side=LEFT)

# functions and associated key bindings
def jump_to_event(event):
    evtSpecified = evtSpecifier.get() # get user specified event number
    if not evtSpecified.isdigit(): return
    # find the minimal total number of events in all 8 channels
    min=999999999
    for ch in range(8):
        if channel[ch]==0:continue # skip empty channel
        if channel[ch].get_visible() and min>n[ch]: min=n[ch]
    # set the current event
    global evt;
    if int(evtSpecified) < 0: evt=0
    elif int(evtSpecified) > min: evt=min-1
    else: evt=int(evtSpecified)
    
    # update the title bar
    title="There are "+str(min)+" events in run "+run
    title+=" (press h for help, q to quit)"
    window.wm_title(title)

    # update canvas
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible(): channel[ch].set_ydata(t[ch][b's'][evt])
    ax.relim(); ax.autoscale_view(); ax.legend(); canvas.draw()

# https://stackoverflow.com/a/42194708/1801749
def discard_modification(event): canvas.get_tk_widget().focus_force()

# https://stackoverflow.com/questions/47475783
evtSpecifier.bind('<Return>', jump_to_event)
evtSpecifier.bind('<Escape>', discard_modification)
runSpecifier.bind('<Escape>', discard_modification)

def show_previous_event():
    min=999999999
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible() and min>n[ch]: min=n[ch]
    global evt; evt-=1
    if evt==-1: evt=min-1
    evtSpecifier.delete(0, END); evtSpecifier.insert(0, str(evt));
    title="There are "+str(min)+" events in run "+run
    title+=" (press h for help, q to quit)"
    window.wm_title(title)
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible(): channel[ch].set_ydata(t[ch][b's'][evt])
    ax.relim(); ax.autoscale_view(); ax.legend(); canvas.draw()

def show_next_event():
    min=999999999
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible() and min>n[ch]: min=n[ch]
    global evt; evt+=1;
    if evt==min: evt=0
    evtSpecifier.delete(0, END); evtSpecifier.insert(0, str(evt));
    title="There are "+str(min)+" events in run "+run
    title+=" (press h for help, q to quit)"
    window.wm_title(title)
    for ch in range(8):
        if channel[ch]==0:continue
        if channel[ch].get_visible(): channel[ch].set_ydata(t[ch][b's'][evt])
    ax.relim(); ax.autoscale_view(); ax.legend(); canvas.draw()

def toggle_ch(event):
    ch=int(event.key)
    if channel[ch]==0: return
    if channel[ch].get_visible(): channel[ch].set_visible(False)
    else: channel[ch].set_visible(True)
    ax.relim(); ax.autoscale_view(); ax.legend(); canvas.draw()

def handle_key_press(event):
    if event.key=="q":
        window.quit()    # stop the main loop
        window.destroy() # necessary on Windows to prevent fatal error
    elif event.key.isdigit(): toggle_ch(event)
    elif event.key=="h": print(hotkeys)
    elif event.key==" ": show_next_event()
    elif event.key=="b": show_previous_event()
    else:
        # handle default matplotlib key bindings
        from matplotlib.backend_bases import key_press_handler
        key_press_handler(event, canvas)

canvas.mpl_connect("key_press_event", handle_key_press)

# give focus to the GUI window in Mac
# https://stackoverflow.com/questions/17774859
from os import system
from platform import system as platform
if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

# If you put window.destroy() here, it will cause an error if the window is
# closed with the window manager.
window.mainloop()
