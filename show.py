#!/usr/bin/env python
usage='python show.py yyyy/mm/dd/HHMM'

hotkeys='''
   --- List of key bindings ---
0-7:        toggle channel 0 to 7
<Space>:    next event
b:          previous event
q:          quit
'''

# get run folder
from sys import argv
if len(argv)<2: print(usage); exit()
folder=argv[1]

print("check ROOT files in "+folder+":")
from os import path
import uproot
t=[0]*8; n=[0]*8; nfile=0; nevt=0
for ch in range(8):
    f=folder+"/wave"+str(ch)+".root"
    if path.exists(f):
        t[ch]=uproot.open(f)['t'].arrays()
        n[ch]=len(t[ch])
        print(f+" contains "+str(n[ch])+" events")
        if n[ch]>0: nfile+=1
        if nevt<n[ch]: nevt=n[ch]
if nfile<1: print("no valid root file in "+folder+", quit"); exit()

title="There are "+str(nevt)+" events in run folder "+folder
title+=" (press h for help, q to quit)"
from tkinter import Tk, Label, Entry, END, TOP, LEFT, BOTH
root = Tk(); root.wm_title(title)

# canvas
from matplotlib.figure import Figure
fig=Figure(); ax=fig.add_subplot()
line=[0]*8
for ch in range(8):
    if n[ch]>0: line[ch],=ax.plot(t[ch][b's'][0],label="channel "+str(ch))
ax.legend()
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
canvas = FigureCanvasTkAgg(fig, master=root); canvas.draw()
canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=1)

# tool bar
from matplotlib.backends.backend_tkagg import NavigationToolbar2Tk
toolbar=NavigationToolbar2Tk(canvas, root)

Label(toolbar, text="Event:").pack(side=LEFT)
evtSpecifier=Entry(toolbar, width=8)
evtSpecifier.insert(0, '0')
evtSpecifier.pack(side=LEFT)
# https://stackoverflow.com/a/42194708/1801749
def discard_modification(event): canvas.get_tk_widget().focus_force()
evtSpecifier.bind('<Escape>', discard_modification)

# functions and associated key bindings
def jump_to_event(event=None):
    evt = evtSpecifier.get()
    if not evt.isdigit(): return
    if int(evt) < 0: evt='0'
    if int(evt) > nevt: evt=str(nevt-1)
    evtSpecifier.delete(0, END); evtSpecifier.insert(0, evt);
    # update canvas
    for ch in range(8):
        if line[ch]==0:continue
        if line[ch].get_visible(): line[ch].set_ydata(t[ch][b's'][int(evt)])
    ax.relim(); ax.autoscale_view(); ax.legend(); canvas.draw()
# https://stackoverflow.com/questions/47475783
evtSpecifier.bind('<Return>', jump_to_event)

def show_previous_event():
    evt = evtSpecifier.get()
    if not evt.isdigit(): return
    evt = str(int(evt)-1)
    evtSpecifier.delete(0, END); evtSpecifier.insert(0, evt);
    jump_to_event()

def show_next_event():
    evt = evtSpecifier.get()
    if not evt.isdigit(): return
    evt = str(int(evt)+1)
    evtSpecifier.delete(0, END); evtSpecifier.insert(0, evt);
    jump_to_event()

def toggle_ch(event):
    ch=int(event.key)
    if ch>7 or ch<0: return
    if line[ch]==0: return
    if line[ch].get_visible(): line[ch].set_visible(False)
    else: line[ch].set_visible(True)
    ax.legend(); canvas.draw()

from matplotlib.backend_bases import key_press_handler
def handle_key_press(event):
    if event.key=="q": root.quit(); root.destroy()
    elif event.key.isdigit(): toggle_ch(event)
    elif event.key=="h": print(hotkeys)
    elif event.key==" ": show_next_event()
    elif event.key=="b": show_previous_event()
    else: key_press_handler(event, canvas) # handle default matplotlib hotkeys
canvas.mpl_connect("key_press_event", handle_key_press)

# give focus to the GUI window in Mac
# https://stackoverflow.com/questions/17774859
from os import system
from platform import system as platform
if platform() == 'Darwin':  # How Mac OS X is identified by Python
    system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')

# If you put root.destroy() here, it will cause an error if the window is
# closed with the window manager.
root.mainloop()
