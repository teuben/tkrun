#! /bin/env python

class PyRun:                                                                     # Begin Class to build GUI
   """class to run functions under different environments"""
   def __init__(self, fun):
      self._fun = fun                                                            # Store the Function
      self._argcnt = fun.func_code.co_argcount                                   # Store Number of Arguments in Function passed to PyRun
      self._radcount = 0                                                         # Variable to work with Radio Widgets
      self._checkcount = 0                                                       # Variable to work with Check Widgets
      self._statecount = 0                                                       # Variable to work with Check Widgets
      self._place = 0                                                            # Variable to help with window geometry
      self._HELPList = []                                                        # List to Store Help Info from Doc String
      self._ifilecount = 0                                                       # Variable to help with Infile Entries
      self._IFileClick = 0                                                       # Variable to help with Infile Entries
      self.IFileTotal = 0                                                        # Variable to help with Infile Entries
      self.Ifieldentry =0                                                        # Variable to help with Infile Entries
      self._ofilecount = 0                                                       # Variable to help with Outfile Entries
      self._OFileClick = 0                                                       # Variable to help with Outfile Entries
      self.OFileTotal = 0                                                        # Variable to help with Outfile Entries
      self.Ofieldentry =0                                                        # Variable to help with Outfile Entries
      
   def GetValues(self):
      """Function to Grab Entries in the GUI"""
      runlist  = []                                                              # Make List to Store Dynamic Variables
      DynPassList = []                                                           # Create Dynamic List to Store Values to be passed
      PassList = []                                                              # Static List of Values to be passed
      radcount = 0                                                               # Iterative Value to find Radio Values
      checkcount = 0                                                             # Iterative Value to find Check Values
      statecount = 0                                                             # Iterative Value of States that Store Check Values
      Passcount = 0                                                              # Store Length of Variable List to be Passed
      
      for a in range(self._argcnt):                                              # Loop over number of Arguments needed to form Dynamic Variables
         
         FieldClassi = "FieldClass = self.Field%d.__class__" %a                  # Create String to store current argument's Widget Type
         exec(FieldClassi)                                                       # Create Dynamic Variable to determine Tkinter Class
         FieldType = str(FieldClass).split('.')                                  # Split Variable into Class alone
         
         if FieldType[1] == "Entry" or FieldType[1] == "Scale":
            Passcount = Passcount+1
            runlist.append("runtmp%d = self.Field%d.get()" %(a,a))
         elif FieldType[1] == "Radiobutton":
            Passcount = Passcount+1
            radcount = radcount + 1
            runlist.append("runtmp%d = self.RadioVar%d.get()" %(a,radcount))
         elif FieldType[1] == "Checkbutton":
            checkcount = checkcount + 1
            statecount = statecount + 1
            DynChkList = "TmpChkList = map((lambda CheckVar%d: CheckVar%d.get()), self._states%d)"\
            %(checkcount,checkcount,statecount)                                  # Make Dynamic List holding boolean values representing selections
            DynChkList = DynChkList + "\nChkList%d = []" %(checkcount)
            exec(DynChkList)
            for b in range(len(TmpChkList)):
               if TmpChkList[b] == '1':
                  runlist.append("ChkList%d.append(self.CheckChange%d[%d])"\
                  %(checkcount,a,b))                                             # Reassign Boolean Value 1 to proper variable
               else:
                  runlist.append("ChkList%d.append(None)" %(checkcount))         # Reassign Boolean Value 0 to None
            runlist.append("runtmp%d = ChkList%d" %(a,checkcount))
            Passcount = Passcount+1
      for b in range(len(runlist)):
         exec(runlist[b])                                                        # Store Values User entered in static variable
      for c in range(Passcount):
         DynPassList.append("PassList.append(runtmp%d)" %c)
         exec(DynPassList[c])
      return PassList                                                            # Pass List of GUI Entries
   
   
   def Runbutton(self):                                                          # Create Function that returns values from GUI to needed program
      """Run Button Function"""
      FncList = self.GetValues()                                                 # Store List of entered GUI values
      FncRunStr = "self._fun("
      for b in range(self._argcnt):
         if b < (self._argcnt - 1):
            FncRunStr = FncRunStr + "FncList[%d]," %b
         else:
            FncRunStr = FncRunStr + "FncList[%d])" %b
      exec(FncRunStr)                                                            # Execute Function Called by User with GUI values

   
   def Quit(self):                                                               # Quit Button Function
      sys.exit()
   
   def GetFile(self,event):
      """Function used to Open Files"""
      if self._IFileClick == self.IFileTotal:                                    # Determine if user has tried to change InFile Entries too many times
         print "Warning! Current Version Only Supports using each InFile Widget once and in Order! Clearing Entries."
         for a in range(self.IFileTotal):
            IFileClear = "self.InFile%d.set('')" %a
            exec(IFileClear)                                                     # Clear all Entries using GetFile Function
         self._IFileClick = 0                                                    # Reset Click Count
         self._ifilecount = 0                                                    # Reset Variable Count
      else:
         currentdir = os.getcwd()
         filename = askopenfilename(initialdir=currentdir)                       # Open File Browser
         if filename:
            DynFileAssign = "self.InFile%d.set('%s')" %(self._ifilecount,filename)
            exec(DynFileAssign)                                                  # Set Entries using GetFile Function to Proper File Name
         self._ifilecount = self._ifilecount+1
         self._IFileClick = self._IFileClick+1

   
   def SaveFile(self,event):
      """Function used to Save Files"""
      if self._OFileClick == self.OFileTotal:                                    # Determine if user has tried to change OutFile Entries too many times
         print "Warning! Current Version Only Supports using each OutFile Widget once and in Order! Clearing Entries."
         for a in range(self.OFileTotal):
            OFileClear = "self.OutFile%d.set('')" %a
            exec(OFileClear)                                                     # Clear all Entries using SaveFile Function
         self._OFileClick = 0                                                    # Reset Click Count
         self._ofilecount = 0                                                    # Reset Variable Count
      else:
         currentdir = os.getcwd()
         filename = asksaveasfilename(initialdir=currentdir)                     # Open File Browser
         if filename:
            DynFileAssign = "self.OutFile%d.set('%s')" %(self._ofilecount,filename)
            exec(DynFileAssign)                                                  # Set Entries using SaveFile Function to Proper File Name
         self._ofilecount = self._ofilecount+1
         self._OFileClick = self._OFileClick+1
         
   def helper(self):
      """Displays a Help Window compiled from the Doc String"""
      helpwindow = Toplevel()                                                    # Create a new widget
      helpwindow.geometry("595x330")
      helpwindow.title("Variable Help")                                          # Name New Window
      text=Text(helpwindow)
      for b in range(len(self._HELPList)):
         FormatHELP = self._HELPList[b].split()                                  # Split Current Line in List of Help Strings For Formatting Reasons
         HelpString = "%s " %str(FormatHELP[0])
         for c in range(len(FormatHELP) - 1):
            HelpString = HelpString + "%s " %str(FormatHELP[c+1])
         text.insert(END,"%s \n" %HelpString)                                    # Print Current List Item in Left Justified Format
      
      scroll = Scrollbar(helpwindow)
      text.config(state=DISABLED,yscrollcommand=scroll.set)
      scroll.config(command=text.yview)
      text.place(x=5,y=5)
      scroll.pack(side=RIGHT,fill=Y)

   def Check(self):
      """Create a Window with some Useful Checks"""
      ArgFlag=0                                                                  # A Flag to alert user about defining variables in a call to function
      DocFlag=0                                                                  # A flag to alert user that Document string does not give information for each variable
      CheckWindow = Toplevel()                                                   # Make a new widget
      CheckWindow.title("Check of Docstring and Arguments")                      # Name the new Window
      CheckWindow.geometry("595x330")
      text=Text(CheckWindow)
      text.insert(END,"Default Arguments Passed to Function:\n")
      
      if len(self._fun.func_code.co_varnames) == len(self._fun.func_defaults):   # Check if Each Argument has been defined in call to function
         for a in range(len(self._fun.func_code.co_varnames)):
            text.insert(END,"%2s= %s \n" %(self._fun.func_code.co_varnames[a],\
            self._fun.func_defaults[a]))
      else:                                                                      # If Each Argument is not Defined, then Display Values found by Docstring
         text.insert(END,"Note: Not all arguments defined in pass to "+\
         "function.Missing values provided by docstring.\n")
         ArgFlag=1
         for b in range(len(self._fun.func_code.co_varnames)):
            text.insert(END,"%2s= %s \n" %(self._fun.func_code.co_varnames[b],\
            self.ValuelistOrder[b]))
      
      text.insert(END,"\nArguments Quoted by Docstring:\n")
      for c in range(len(self.VarlistOrder)):
         if self.VarlistOrder[c] != 'BLANK':
            text.insert(END,"%2s= %s \n" %(self.VarlistOrder[c],self.ValuelistOrder[c]))
         else:
            text.insert(END,"%2s= (Not Defined in Doc String) \n" %(self._fun.\
            func_code.co_varnames[c]))
            DocFlag=1
            print DocFlag
      
      text.insert(END,"\nArguments To be passed to function by GUI:\n")
      ArgCheckList = self.GetValues()                                          # get Current Values in the GUI
      for d in range(len(self._fun.func_code.co_varnames)):
         text.insert(END,"%2s= %s \n" %(self._fun.func_code.co_varnames[d],ArgCheckList[d]))
      if ArgFlag == 1:
         text.insert(END,"\nFor best results, please pass defaults values to each argument.")
      if DocFlag == 1:
         text.insert(END,"\nFor best results, please write a complete document string.")
      
      scroll = Scrollbar(CheckWindow)
      text.config(state=DISABLED,yscrollcommand=scroll.set)
      scroll.config(command=text.yview)
      text.place(x=5,y=5)
      scroll.pack(side=RIGHT,fill=Y)
      
   def tkrun(self):                                                              # Function to Create GUI

      DISP        = []                                                           # Create List of Dynamic Tkinter Classes depending on input function
      typelist    = []                                                           # List of relevant lines from the help document
      Fieldlist   = []                                                           # List of Tkinter Class for each variable
      Varlist     = []                                                           # Store which variable each line corresponds to
      Paramlist   = []                                                           # Store list of Parameters used for certain Tkinter Classes
      Valuelist   = []                                                           # Store value in document string for each variable
      self.FieldlistOrder   = []                                                 # Reordered Fieldlist to represent how variables are called in function
      self.VarlistOrder     = []                                                 # Reordered varlist to represent how variables are called in function
      self.ParamlistOrder   = []                                                 # Reordered Paramlist to represent how variables are called in function
      self.ValuelistOrder   = []                                                 # Reordered Valuelist to represent how variables are called in function
      
      if self._fun.__doc__ == None:
         print "ERROR: Missing Document String"
         self.Quit()
      document = self._fun.__doc__                                               # Read in Document String for Function
      doclist = document.split('\n')
      for a in range(len(doclist)):
         SpltDoci = doclist[a].split('#> ')
         if len(SpltDoci) > 1:
            typelist.append(SpltDoci[1])                                         # Store Relevant lines from Document String
         else:
            self._HELPList.append(SpltDoci[0])
      for b in range(len(typelist)):
         typelisti = typelist[b].split(' ')
         typelistj = typelisti[1].split('=')
         Fieldlist.append(typelisti[0])
         Varlist.append(typelistj[0])
         
         if len(typelistj) > 1:                                                  # Store a value for each variable depending on Tkinter class
            Valuelist.append(typelistj[1])
         else:
            Valuelist.append('')
         
         if len(typelisti) > 2:                                                  # Store Parameters for certain Tkinter Classes
            Paramlist.append(typelisti[2])
         else:
            Paramlist.append('')
            
      for c in range(self._fun.func_code.co_argcount):                           # Begin loop to sort document information into order it appears in function
         flag = 0
         self.FieldlistOrder.append('')
         self.VarlistOrder.append('')
         self.ParamlistOrder.append('')
         self.ValuelistOrder.append('')
         for d in range(len(Varlist)):
            if self._fun.func_code.co_varnames[c] == Varlist[d]:
               self.VarlistOrder[c]   = Varlist[d]
               self.FieldlistOrder[c] = Fieldlist[d]
               self.ParamlistOrder[c] = Paramlist[d]
               if Valuelist[d] != '':
                  self.ValuelistOrder[c] = Valuelist[d]
               else:
                  self.ValuelistOrder[c] = 'BLANK'
               flag = 1
            elif self._fun.func_code.co_varnames[c] != Varlist[d] and flag == 0:
               self.VarlistOrder[c]   = 'BLANK'
               self.FieldlistOrder[c] = 'BLANK'
               self.ParamlistOrder[c] = 'BLANK'
               self.ValuelistOrder[c] = 'BLANK'
            else:
               pass
      for e in range(self._fun.func_code.co_argcount):
         if self.VarlistOrder[e] == 'BLANK':
            self.VarlistOrder[e] = self._fun.func_code.co_varnames[e]
         else:
            pass
      
      root = Tk()                                                             # Create Parent Window
      root.title("%s" %self._fun.__name__)                                    # Name Window with Function Name
      topmenu = Menu(root,bd=1,activeborderwidth=0)
      root.config(menu=topmenu,background="Grey")
      
      paramlength = 0
      extrarow    = 0
      for e in range(len(self.ParamlistOrder)):
         paramtmp = self.ParamlistOrder[e].split(',')
         if len(paramtmp) > paramlength:
            paramlength = len(paramtmp)                                       # Store the number maximum number of parameters for one variable
         else:
            pass
         if len(paramtmp) > 14:
            extrarow = extrarow + long(len(paramtmp)/14)                      # Find how many extra lines on the window are needed for extra rows of parameters
      
      if (self._fun.func_code.co_argcount+extrarow) == 15:                    # Set default window height if over sMaximum number of lines for a window.
         WINheight = 1000
      else:
         WINheight = (50+40*self._fun.func_code.co_argcount+40*extrarow)      # Adjust Height if under maximum.
      
      if paramlength >= 14:                                                   # Set default window width if over sMaximum number of lines for a window.
         WINwidth = 1250
      else:
         if paramlength <= 5:                                                 # Set minimum window width
            WINwidth = 515
         else:
            WINwidth = (475+75*(paramlength-5))                               # Set intermediate window widths
      root.geometry("%dx%d" %(WINwidth,WINheight))                            # Change Size of GUI to represent amount of variables
      
      menu = Menu(root)                                                       # Create a menu
      root.config(menu=menu)
      filemenu = Menu(menu)
      helpmenu = Menu(menu)
      menu.add_cascade(label="Run", menu=filemenu)
      filemenu.add_command(label="Run", command=self.Runbutton)
      filemenu.add_command(label="Check", command=self.Check)
      filemenu.add_command(label="Quit", command=self.Quit)
      menu.add_cascade(label="Help", menu=helpmenu)
      helpmenu.add_command(label="Help", command=self.helper)
      
      frame=Frame(root,bd=2)
      canvas = Canvas(frame)
      scrollbar = Scrollbar(frame)
      canvas.config(yscrollcommand=scrollbar.set)
      canvas['width'] = WINwidth
      canvas['height'] =(50+40*self._fun.func_code.co_argcount+40*extrarow)
      canvas['scrollregion'] = (0, 0, 0, (70+40*self._fun.func_code.co_argcount+40*extrarow))
      canvas.place(x=0,y=0)
      scrollbar.config(command=canvas.yview)
      scrollbar.pack(side=RIGHT,fill=Y)
      frame.pack()
      

      for a in range(self._fun.func_code.co_argcount):
         textvar = "tmp%d = StringVar()\n" %a                                          # Initilize Dynamic Variable to store default values to appear in GUI
         
         if len(self._fun.func_defaults) == (self._fun.func_code.co_argcount):         # If each variable defined in function call, default to fnc. Doc String used if not.
            textvar = textvar + "tmp%d.set(%s.%s.func_defaults[%d])"\
            %(a,self._fun.__module__,self._fun.__name__,a)
         else:
            textvar = textvar + "tmp%d.set(self.ValuelistOrder[%d])" %(a,a)
         
         exec(textvar)                                                                 # Make Static Version of each Dynamic Variable

         DISP.append("self.Label%d = Label(canvas,text='%s:')"\
         %(a,self._fun.func_code.co_varnames[a]))   # Create Label for each Variable
         DISP.append("self.Label%d.place(x=10,y=(5+%d))" %(a,(a*40+40*self._place)))
         
         if self.FieldlistOrder[a] == "ENTRY":                                         # Make Entry Class if quoted in Docstring for current Variable
            DISP.append("self.Field%d = Entry(root,textvariable=\
            tmp%d,width=50,bd=1,background='White',selectbackground='Black')" %(a,a))
            DISP.append("self.Field%d.place(x=100,y=(5+%d))" %(a,(a*40+40*self._place)))
         elif self.FieldlistOrder[a] == "SCALE":                                       # Make Scale Class if quoted in Docstring for current Variable
            Scalelist = self.ParamlistOrder[a].split(':')
            
            DISP.append("self.Field%d = Scale(root,from_=%d,to=%d,resolution=%f,\
            orient=HORIZONTAL,length=350,width=9,font=8,variable=tmp%d)"\
            %(a,float(Scalelist[0]),float(Scalelist[1]),float(Scalelist[2]),a))
            DISP.append("self.Field%d.place(x=100,y=(5+%d))" %(a,(a*40+40*self._place)))
         elif self.FieldlistOrder[a] == "CHECK":                                       # Make Check Class if quoted in Docstring for current Variable
            Checklist = self.ParamlistOrder[a].split(',')
            self._checkcount = self._checkcount+1
            self._statecount = self._statecount+1
            
            DynCheck = "self.CheckChange%d = Checklist" %a
            DynStates = "self._states%d = []" %self._statecount
            exec(DynCheck)
            exec(DynStates)
            
            for b in range(len(Checklist)):
               if type(Checklist[b]) == str: 
                  DISP.append("self.CheckVar%d = StringVar()" %(self._checkcount))
               elif type(Checklist[b]) == float:
                  DISP.append("self.CheckVar%d = FloatVar()" %(self._checkcount))
               else:
                  DISP.append("self.CheckVar%d = IntVar()" %(self._checkcount))
               
               DISP.append("self.Field%d = Checkbutton(root, text='%s',variable=self.CheckVar%d)" %(a,str(Checklist[b]),self._checkcount))
               DISP.append("self.Field%d.place(x=100+(75*%d),y=(5+%d))" %(a,(b-14*long(b/14)),(a*40+long(b/14)*40+40*self._place)))
               DISP.append("self._states%d.append(self.CheckVar%d)" %(self._statecount,self._checkcount))
            
            if len(Checklist) >= 14:
               self._place=self._place+long(len(Checklist)/14)
         elif self.FieldlistOrder[a] == "RADIO":                                       # Make Radio Class if quoted in Docstring for current Variable
            Radiolist = self.ParamlistOrder[a].split(',')
            self._radcount = self._radcount+1
                        
            for c in range(len(Radiolist)):
               if type(Radiolist[c]) == str: 
                  DynRadioVar = "self.RadioVar%d = StringVar()" %(self._radcount)
               elif type(Radiolist[c]) == float:
                  DynRadioVar = "self.RadioVar%d = FloatVar()" %(self._radcount)
               else:
                  DynRadioVar = "self.RadioVar%d = IntVar()" %(self._radcount)
               exec(DynRadioVar)
               DISP.append("self.Field%d = Radiobutton(root, text='%s', value='%s',\
               variable=self.RadioVar%d)" %(a,str(Radiolist[c]),str(Radiolist[c]),\
               self._radcount))
               DISP.append("self.Field%d.place(x=100+(60*%d),y=(5+%d))"\
               %(a,(c-14*long(c/14)),(a*40+long(c/14)*40+40*self._place)))
            
            if len(Radiolist) >= 14:
               self._place=self._place+long(len(Radiolist)/14)
         elif self.FieldlistOrder[a] == "INFILE":                                      # Make Infile if quoted in Docstring for current Variable
            DynInFile = "self.InFile%d = StringVar()" %(self.Ifieldentry)
            exec(DynInFile)
            DISP.append("self.Field%d = Entry(root,textvariable=self.InFile%d,width=50,bd=1,background='White',selectbackground='Black')"%(a,self.Ifieldentry))
            DISP.append("self.Field%d.bind('<Button-3>', self.GetFile)" %a)
            DISP.append("self.Field%d.place(x=100,y=(5+%d))" %(a,a*40+40*self._place))
            DISP.append("self.Label%d_%d = Label(root,text='In',relief=RIDGE)" %(a,a))
            DISP.append("self.Label%d_%d.place(x=460,y=(5+%d))" %(a,a,a*40+40*self._place))
            self.Ifieldentry = self.Ifieldentry+1
         elif self.FieldlistOrder[a] == "OUTFILE":                                     # Make Outfile if quoted in Docstring for current Variable
            DynOutFile = "self.OutFile%d = StringVar()" %(self.Ofieldentry)
            exec(DynOutFile)
            DISP.append("self.Field%d = Entry(root,textvariable=self.OutFile%d,width=50,bd=1,background='White',selectbackground='Black')" %(a,self.Ofieldentry))
            DISP.append("self.Field%d.bind('<Button-3>', self.SaveFile)" %a)
            DISP.append("self.Field%d.place(x=100,y=(5+%d))" %(a,a*40+40*self._place))
            DISP.append("self.Label%d_%d = Label(root,text='Out',relief=RIDGE)" %(a,a))
            DISP.append("self.Label%d_%d.place(x=460,y=(5+%d))" %(a,a,a*40+40*self._place))
            self.Ofieldentry = self.Ofieldentry+1
         else:                                                                         # Default to Entry Class if not in Docstring
            DISP.append("self.Field%d = Entry(root,textvariable=tmp%d,width=50,bd=1,background='White',selectbackground='Black')" %(a,a))
            DISP.append("self.Field%d.place(x=100,y=(5+%d))" %(a,a*40+40*self._place))
      for d in range(len(DISP)):
         exec(DISP[d])

      SelfContent = dir(self)                                                          # Make a list with contents of self
      for a in range(self._argcnt):                                                    # Determine how many Infiles and Outfiles there are
         for b in range(len(SelfContent)):
            FieldStrIN = "InFile%d" %a
            FieldStrOUT = "OutFile%d" %a
            if str(SelfContent[b]) == FieldStrIN:
               self.IFileTotal = self.IFileTotal+1
            if str(SelfContent[b]) == FieldStrOUT:
               self.OFileTotal = self.OFileTotal+1

      root.mainloop()

from Tkinter import *
from tkFileDialog import askopenfilename
from tkFileDialog import asksaveasfilename
import os

if __name__ == '__main__':
   inFileFull  = sys.argv[1]
   inFileSplt  = inFileFull.split('.')
   inFile      = inFileSplt[0]
   
   importstr = "import %s" %inFile
   exec(importstr)
   
   command = "p = PyRun(%s)\n" %inFileFull
   command = command + "p.tkrun()"
   exec(command)
