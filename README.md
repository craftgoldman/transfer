Transfer v3.3
=====================================================================
Copyright: Dmitry Golubev
E-mail:    dagolubev@gmail.com


What is Transfer
---------------------------------------------------------------------
Transfer  is  the  universal  customizable utility for data transfer/
conversion from one database to another.

Program is written in Microsoft Visual C++ with ODBC API, wich allows 
to reach  the maximum productivity in massive data blocks processing. 
Program  unification  is  based on standardized interface provided by 
ODBC  drivers. Wide selection of existing ODBC drivers makes Transfer 
the  universal  tool to work with both file databases (dBase, Access) 
and relational databases (Oracle, Sybase, Microsoft SQL). In its work 
Transfer  uses  internal format of data representation, that makes it 
independent from specific data types peculiar to different databases. 
Embedded  internal  data  definition language allows to customize the 
program to perform any tasks for information transferring/conversion.


Main features
---------------------------------------------------------------------
- Work with any databases via ODBC interface.
- High performance in data processing.
- SQL query language support.
- Automatic data type convesion during transfer.
- Embedded flexible data structure (profiles) definition language.
- Support of external arguments for a data profile.
- Convenient data profile editor (GUI).
- Stored database procedures' calls during transfer.
- Trigger-based mechanism to make the data processing scenario.
- Possibility to process master-slave tables in one logical block.
- Internal text data convertor from ANSI to OEM and vice versa.
- Options to run from command line.
- Logging all actions in a protocol file.
- Optional SQL-commands execution tracing.
- Automatic message files creation upon the transfer completion.
- Possibility to run automatic file exchange with a remote host.
- Internal LZW data archiver.
- Multilingual interface.


What's New
---------------------------------------------------------------------
Please see HISTORY.TXT for new features and bug fixes.


Directories and files
---------------------------------------------------------------------
.\In         - import directory
.\Out        - export directory
.\Ppl        - data profiles directory
.\Receive    - directory to receive files from a remote host
.\Send       - directory to send files to a remote host
.\Txt        - protocol and message files directory

Trn33enu.dll - english language support
Trans33.exe  - Transfer application
unistall.exe - Transfer uninstaller
Trans33.hlp  - Transfer help file
Trans33.chm  - Compiled HTML help file
Trans33.ini  - Transfer config file
unistall.ini - uninstall log file
License.txt  - license agreement
History.txt  - changes list
Order.txt    - ordering information


Installation
---------------------------------------------------------------------
Extract  .zip file to the temporary directory on a disk, then execute 
TRN33setup_eng.exe.


Removing Transfer
---------------------------------------------------------------------
Execute 'Uninstall Transfer' in the Transfer Start Menu folder or open
'Add/Remove Programs' in Control Panel and double-click 'Transfer 3.3' 
in the list box.


Feedback
---------------------------------------------------------------------
For feedback, questions and any comments: dagolubev@gmail.com

