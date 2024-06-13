import tkinter as tk
import mmap
import struct
import ctypes

leds = ["MdmNumber", "RX","TX", "DCD", "RTS", "CTS", "DTR", "DSR", "ST", "Modem Name" ]

   
window = tk.Tk()
window.geometry('600x400')
fx = tk.Frame(master=window)    
gtx = []
for F in range(0,8):
    gtx.append( [] )
    for I in range(len(leds)):
        k = '{} '.format(leds[I])
        gtx[F].append( tk.Label(
                fx,
                text=k,
                foreground="white",  # Set the text color to white
                background="black"  # Set the background color to black)
            )
        )
        gtx[F][I].grid(row=F,column=I)
# fx.grid(row=F)        
fx.pack()    


def updMdm(F,x):
    for I in range(len(leds)):
        gtx[F+1][I].config( text=x[I], background= "green" if x[I] == 1 else "gray" if I > 7 else "red" ) 

def mmap_io(filename):
    with open(filename, mode="rb") as file_obj: #, encoding="utf8"
        with mmap.mmap(file_obj.fileno(), length=1120, access=mmap.ACCESS_READ) as mmap_obj:
            text = mmap_obj.read()
            print(len(text))
            for I in range(0,7):
                # (rx,tx,dcd,rts,cts,dtr,dsr,st,nm)
                stTxt = I*140
                (rx,tx,dcd,rts,cts,dtr,dsr,st,nm) = struct.unpack("ccccccci128s",text[stTxt:stTxt+140])
                X = (I, rx,tx,dcd,rts,cts,dtr,dsr,st,ctypes.string_at(nm,128).decode('utf-8') )
                print(rx,tx,dcd,rts,cts,dtr,dsr,st, ctypes.string_at(nm,128).decode('utf-8'))
                updMdm(I,X)


mmap_io("mmMDMx.dsply")
