Project Name
--------------
.MATE- A Guide to Menstruation

Original Author
---------------
Mihir Patel

Build Status
---------------
1) Current: Experimental
2) OS:	Windows

Project Description
-------------------
Following program is a mini Menstrual Guide. This program
reads from a text file, which contains most searched
queries of this topic, and uses user input to find the 
necessary information. User can interact with the program 
by typing statements. The primary audience of this program
is ill-informed individuals who want to explore more about 
female menstruation. However, please note that everyone and 
anyone is open to use it as they please.Then there is a 
Calendar tool, which user can users get their next period. It 
allows prompts as well, and will guideyou accordingly. 

For UML diagram and other implementation, please do check ProjectDescription.docx

Motivation
-----------
I grew up in a very female-dominated household with male to 
female ratio, 8:3. Being in this kind of family, yet not 
knowing what Period is until I got out of high school felt 
little insane to me. We're Living in the 21st century, yet
Menstruation is still treated as taboo. There is zero 
initiative given to this topic. Men don't want to hear 
about it, women don't want to talk about it. That's why 
there are many men who are often ill-informed of this topic. 

By making this program, I wanted to encourage conversation 
with 52% of our world population- Women.

Prerequisites
-------------
A GCC or G++ console support

Assembling the Files
--------------------
Place Period.h, CalendarIO.h, Menstrual_Guide.h, Log_IO.h
header files, along with their associated implemenation 
files (*.cpp) files into one folder. Next place 
Menstrual_archive.txt, users.txt and main.cpp in that folder also.

Getting Started
---------------
1) Please read "Navigation" module towards the bottom first.

2) When using a compiler:  
    (a) Open a new project, upload the header files in the Header 
     Files section. Place their implementation files in the
     Source Files section.    
    (b) Add the driver file main.cpp in the Source 
     files section.  
    (c) Hit run or compile & run.  
    
3) When using a terminal:  
    (a) Open the terminal. "cd" to where you believe files are.  
    (b) Type "ls" to locate the files. See if Period.cpp, 
      Menstrual_Guide.cpp, Calendar_IO.cpp, Log_IO.cpp and 
      main.cpp are in the directory, if not please
      place them in a directory. Repeat the step.  
    (c) Then use "g++ -c *.cpp -o dotMate_Test" then press enter.  
    (d) Then "./dotMate_Test", hit enter.  
    (e) Program will execute.  
    
    
Running the program
-------------------
This program is purposefuly designed to not have any
selection menu. So, every interaction between you and the
program should be genuine. The program interaction is
very simple. For instance, if you want to say "what is a
Period?" Or say, "can a womans period change?!"
Then you would type that, entirely, no special format 
is needed. You may make a spelling error, or accidenently 
hit number,and misplace a punctuation. Or, you may put many 
spaces. Fret not, this program can handle these types of 
user errors. However, please not that excesssive
spelling erros does lead to bad answers. It is advised to
double check what you want to ask. Your input matters.

Where your input is needed in an exact manner, the program 
would tell you. Or it would react in an obvious way which
you weren't expecting. In these casses, unfortunately,
you can only do, what's being asked from you.

Moreover, if you would like to see next period day, say 
"Hey! I want to calculate my next period?" Or, you can say
"I am planning a vacation with my wife so....?" When you
type these type of phrases, the program will prompt you for 
information and in the end it will make you a calendar 
which you can modify, print and plan your family vacation/ 
resort trip according to what your relative and you see fit. 

When you're done & want close the program, you can simply 
close it,or can type, "Exit the program! Bye!" And the
program will close. 

Contribute
------------
Please use github for collaboration and 
contact. Feedbacks, or comments (angry or jolly) are always
important to me.


Code Style
----------
Using standard hanging indentation of C/C++
    
Special Thanks
--------------
Mr. Denis Laeskar, PhD Candidate, USF  
Dr. Tempestt Neal, PhD, USF  
Mr. Mohamed Ebraheem, Undergraduate, USF  
    
License
-----------
Open Source

Navigation
-----------
Hello! Since I couldn't put everything from internet into one 
place (in Menstrual_Guide.txt), the program would best respond 
if you ask these or questions that sound similar to these. 
Offcourse, you can still make spelling mistakes, leave more 
punctuations/spaces/numbers than expected. Sorry for any 
inconvenience. These are few of the most asked questions 
online about Menstruation:  
  
  
What is a period?        
What is menstrual cycle?  
How long is typical menstrual cycle?     
Why does period happen?  
What is the point of getting period?      
Can a womans period change?      
What other way can a womans period change?         
Does not ovulating makes difference?     
What is considered to be normal?   
What is normal amount of bleeding during my period?        
When should i see a doctor?       
How much period does one person has in one lifetime?      
What is normal menstrual cycle?   
What are premenstrual symptoms?    
Tell me more about premenstrual symptoms!          
What is normal menstrual cycle?  
Is it true that a woman is only fertile for about 7 days every month?      
What is ovulation?   
When is the best time to get pregnant?    
How to avoid unwanted pregnancy?  
What happens during the typical 28 day menstrual cycle?   
What is luteinizing hormone?  
How does my menstrual cycle change as woman gets older?    
Why should i keep track of my menstrual cycle?   
When does a girl usually get her first period?  
When should a girl see a doctor?  
How long does a woman usually have periods?   
When does menopause happen?        
Does period stops during pregnancy?      
What are symptoms of heavy menstrual bleeding?   
How often should i change my pad or tampon?  
How often should i change menstrual cup, sponge or period panties?       
What is toxic shock syndrome?  
Can tampons cause toxic shock syndrome?  
Are pads better than tampons?     
What are symptoms of toxic shock syndrome or tss?          
How does the menstrual cycle affect other health problems?         
What is menopause phase?  
What causes menopause?    
What is perimenopause phase?       
What is progesterone hormone?  
What is estrogen hormone?          
What is premenstrual syndrome or pms?        
Does pms change with age?   
What causes pms?  
What is premenstrual dysphoric disorder or pmdd?   
Which type of food to avoid during period?        
What to do when i get my first period?     
Will anyone know i am on my period?        
Can i go swimming or play sports?       
  
    
To access a calendar tool, when the prompt:  

"What would you like to ask?" appears,

Type a sentence something like:  

"I want a Calendar tool...." or "My partner and I planning a vacation....."
