/*******************************************************************
          Copyright (C) 1986 by Manuel E. Bermudez
          Translated to C - 1991
********************************************************************/

#include <stdio.h>
#include <header/CommandLine.h>
#include <header/Open_File.h>
#include <header/Table.h>
#include <header/Text.h>
#include <header/Error.h>
#include <header/String_Input.h>
#include <header/Tree.h>
#include <header/Code.h>
#include <header/CodeGenerator.h>  
#include <stdlib.h>


#define LeftMode 0
#define RightMode 1

    /*  ABSTRACT MACHINE OPERATIONS  */
#define    NOP          1   /* 'NOP'       */
#define    HALTOP       2   /* 'HALT'      */      
#define    LITOP        3   /* 'LIT'       */
#define    LLVOP        4   /* 'LLV'       */
#define    LGVOP        5   /* 'LGV'       */
#define    SLVOP        6   /* 'SLV'       */
#define    SGVOP        7   /* 'SGV'       */
#define    LLAOP        8   /* 'LLA'       */
#define    LGAOP        9   /* 'LGA'       */
#define    UOPOP       10   /* 'UOP'       */
#define    BOPOP       11   /* 'BOP'       */     
#define    POPOP       12   /* 'POP'       */  
#define    DUPOP       13   /* 'DUP'       */
#define    SWAPOP      14   /* 'SWAP'      */
#define    CALLOP      15   /* 'CALL'      */
#define    RTNOP       16   /* 'RTN'       */
#define    GOTOOP      17   /* 'GOTO'      */
#define    CONDOP      18   /* 'COND'      */
#define    CODEOP      19   /* 'CODE'      */
#define    SOSOP       20   /* 'SOS'       */
#define    LIMITOP     21   /* 'LIMIT'     */

    /* ABSTRACT MACHINE OPERANDS */

         /* UNARY OPERANDS */
#define    UNOT        22   /* 'UNOT'     */
#define    UNEG        23   /* 'UNEG'     */
#define    USUCC       24   /* 'USUCC'    */
#define    UPRED       25   /* 'UPRED'    */
         /* BINARY OPERANDS */
#define    BAND        26   /* 'BAND'     */
#define    BOR         27   /* 'BOR'      */
#define    BPLUS       28   /* 'BPLUS'    */
#define    BMINUS      29   /* 'BMINUS'   */
#define    BMULT       30   /* 'BMULT'    */
#define    BDIV        31   /* 'BDIV'     */
#define    BEXP        32   /* 'BEXP'     */
#define    BMOD        33   /* 'BMOD'     */
#define    BEQ         34   /* 'BEQ'      */
#define    BNE         35   /* 'BNE'      */
#define    BLE         36   /* 'BLE'      */
#define    BGE         37   /* 'BGE'      */
#define    BLT         38   /* 'BLT'      */
#define    BGT         39   /* 'BGT'      */
         /* OS SERVICE CALL OPERANDS */
#define    TRACEX      40   /* 'TRACEX'   */
#define    DUMPMEM     41   /* 'DUMPMEM'  */
#define    OSINPUT     42   /* 'INPUT'    */
#define    OSINPUTC    43   /* 'INPUT'    */
#define    OSOUTPUT    44   /* 'OUTPUT'   */
#define    OSOUTPUTC   45   /* 'OUTPUT'   */
#define    OSOUTPUTL   46   /* 'OUTPUTL'  */
#define    OSEOF       47   /* 'EOF'      */

         /* TREE NODE NAMES */
#define    ProgramNode  48   /* 'program'  */
#define    TypesNode    49   /* 'types'    */
#define    TypeNode     50   /* 'type'     */
#define    DclnsNode    51   /* 'dclns'    */
#define    DclnNode     52   /* 'dcln'     */
#define    IntegerTNode 53   /* 'integer'  */
#define    BooleanTNode 54   /* 'boolean'  */
#define    BlockNode    55   /* 'block'    */
#define    AssignNode   56   /* 'assign'   */
#define    OutputNode   57   /* 'output'   */ 
#define    IfNode       58   /* 'if'       */
#define    WhileNode    59   /* 'while'    */
#define    NullNode     60   /* '<null>'   */
#define GTNode          61   /* '>'        */
#define LTNode          62   /* '<'        */
#define GENode          63   /* '>='       */
#define NENode          64   /* '<>'       */
#define EQNode          65   /* '='        */
#define LENode          66   /* '<='       */
#define PlusNode        67   /* '+'        */
#define MinusNode       68   /* '-'        */
#define ORNode          69   /* 'or'       */
#define MODNode         70   /* 'mod'      */
#define ANDNode         71   /* 'and'      */
#define PRODNode        72   /* '*'        */
#define DIVNode         73   /* '/'        */
#define NOTNode         74   /* 'not'      */
#define UMinusNode      75   /* '-'        */
#define POWNode         76   /* '**'       */
#define ReadNode        77   /* 'read'     */
#define EOFNode         78   /* 'eof'      */
#define TRUENode        79   /* 'true'     */
#define FALSENode       80   /* 'false'    */
#define IntegerNode     81   /* '<integer>'*/
#define IdentifierNode  82   /* '<identifier>'  */
#define RepeatNode      83   /* 'repeat'   */
#define LoopNode        84   /* 'loop'     */
#define ExitNode        85   /* 'exit'     */
#define SwapNode        86   /* '<swap>'   */
#define UptoNode        87   /* '<upto>'   */
#define CaseNode        88   /* 'case'     */
#define CaseClauseNode  89   /* '<case_clause'> */
#define RangeNode       90   /* 'range''..'     */
#define OtherwiseNode   91   /* '<otherwise>'   */
#define DownToNode      92   /* '<downto>' */
#define CharacterTNode  93   /* 'char'     */
#define CharacterNode   94   /* '<char>    */
#define ConstsNode      95   /* '<consts>  */
#define ConstNode       96   /* 'const'    */
#define LitNode         97   /* 'lit'      */
#define SuccNode        98   /* 'succ'     */
#define PredNode        99   /* 'pred'     */
#define ChrNode        100   /* 'chr'      */
#define OrdNode        101   /* 'ord'      */
#define StringNode     102   /* '<string>' */

#define NumberOfNodes  102

typedef int Mode;
typedef TreeNode UserType;

FILE *CodeFile;
char *CodeFileName;
Clabel HaltLabel;

UserType TypeInteger, TypeBoolean, TypeCharacter;

char *mach_op[] = 
    {"NOP","HALT","LIT","LLV","LGV","SLV","SGV","LLA","LGA",
     "UOP","BOP","POP","DUP","SWAP","CALL","RTN","GOTO","COND",
     "CODE","SOS","LIMIT","UNOT","UNEG","USUCC","UPRED","BAND",
     "BOR","BPLUS","BMINUS","BMULT","BDIV","BEXP","BMOD","BEQ",
     "BNE","BLE","BGE","BLT","BGT","TRACEX","DUMPMEM","INPUT",
     "INPUTC","OUTPUT","OUTPUTC","OUTPUTL","EOF"};

/****************************************************************** 
   add new node names to the end of the array, keeping in strict order
   as defined above, then adjust the j loop control variable in
   InitializeNodeNames(). 
*******************************************************************/
char *node_name[] = { "program", "types", "type", "dclns",
                 "var", "integer", "boolean", "block",
                 "assign", "output", "if", "while", 
                 "<null>", ">", "<", ">=",
                 "<>", "=", "<=", "+",
                 "-", "or", "mod", "and",
                 "*", "/", "not", "neg",
                 "pow", "read", "eof", "<true>", "<false>",
                 "<integer>", "<identifier>" ,
                 "repeat", "loop", "exit", "<swap>",
                 "<upto>", "case", "<case_clause>", "<range>",
                 "<otherwise>", "<downto>", "char", "<char>",
                 "<consts>", "const", "lit", "succ",
                 "pred", "chr", "ord", "<string>" 
                };

void intrinsicFunction(TreeNode T, Clabel CurrLabel, int UOP);
void input (TreeNode T, Clabel CurrLabel);
void output(TreeNode T, Clabel CurrLabel);
void specialKeys(int ascii, Clabel CurrLabel);

void CodeGenerate(int argc, char *argv[])
{
   int NumberTrees;

   InitializeCodeGenerator(argc,argv);
   Tree_File = Open_File("_TREE", "r"); 
   NumberTrees = Read_Trees();
   fclose (Tree_File);                     

   TypeBoolean = Child(Child (RootOfTree(1), 2),1);
   TypeInteger = Child(Child (RootOfTree(1), 2),2);
   TypeCharacter = Child(Child (RootOfTree(1), 2),3);

   HaltLabel = ProcessNode (RootOfTree(1), NoLabel);
   CodeGen0 (HALTOP, HaltLabel); 
   
   CodeFile = Open_File (CodeFileName, "w");
   DumpCode (CodeFile);
   fclose(CodeFile); 

   if (TraceSpecified)
      fclose (TraceFile);

/****************************************************************** 
  enable this code to write out the tree after the code generator
  has run.  It will show the new decorations made with MakeAddress().
*******************************************************************/


   Tree_File = fopen("_TREE", "w");  
   Write_Trees();
   fclose (Tree_File);                   
}


void InitializeCodeGenerator(int argc,char *argv[])
{
   InitializeMachineOperations();
   InitializeNodeNames();
   FrameSize = 0;
   CurrentProcLevel = 0;
   LabelCount = 0;
   CodeFileName = System_Argument("-code", "_CODE", argc, argv); 
}


void InitializeMachineOperations(void)
{
   int i,j;

   for (i=0, j=1; i < 47; i++, j++)
      String_Array_To_String_Constant (mach_op[i],j);
}



void InitializeNodeNames(void)
{
   int i,j;

   for (i=0, j=48; j <= NumberOfNodes; i++, j++)
      String_Array_To_String_Constant (node_name[i],j);
}

String MakeStringOf(int Number)
{
   Stack Temp;

   Temp = AllocateStack (50);
   ResetBufferInTextTable();
   if (Number == 0)
      AdvanceOnCharacter ('0');
   else
   {
      while (Number > 0)
      {
         Push (Temp,(Number % 10) + 48);
         Number /= 10;
      }

      while ( !(IsEmpty (Temp)))
         AdvanceOnCharacter ((char)(Pop(Temp)));
   }   
   return (ConvertStringInBuffer()); 
}



void Reference(TreeNode T, Mode M, Clabel L)
{
   int Addr ,OFFSET;
   String  Op;
   int Type;

   Addr = Decoration(Decoration(T));
   OFFSET = FrameDisplacement (Addr) ;


   switch (M)
   {
      case LeftMode  :  DecrementFrameSize();
                        if (ProcLevel (Addr) == 0) 
                           Op = SGVOP;
                        else
                           Op = SLVOP;
                    break;
      case RightMode :  IncrementFrameSize();
                        if (ProcLevel (Addr) == 0) 
                           Op = LGVOP;
                    else
                           Op = LLVOP;
            Type = Decoration(Child(Decoration(T),1));
            if(NodeName(Type) == ConstNode || NodeName(Type) == LitNode){
              Op = LITOP;
            };
                break;
   }
   /* ref de asignación de const y lit */
   if(M == RightMode){
       /* lit */
       if(NodeName(Type) == LitNode){
          CodeGen1 (Op, MakeStringOf(OFFSET), L);
       /* const */
       }else if(NodeName(Type) == ConstNode){
          CodeGen1 (Op, OFFSET, L);
       }else{
          CodeGen1 (Op,MakeStringOf(OFFSET),L);
       }
   }else{
      CodeGen1 (Op,MakeStringOf(OFFSET),L);
   }
}

int NKids (TreeNode T)
{
   return (Rank(T));

};

void Expression (TreeNode T, Clabel CurrLabel)
{
   int Kid;
   Clabel Label1;

   if (TraceSpecified)
   {
      fprintf (TraceFile, "<<< CODE GENERATOR >>> Processing Node ");
      Write_String (TraceFile, NodeName (T) );
      fprintf (TraceFile, " , Label is  ");
      Write_String (TraceFile, CurrLabel);
      fprintf (TraceFile, "\n");
   }

   switch (NodeName(T))
   {
      case LENode :
      case PlusNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         if (NodeName(T) == LENode)
            CodeGen1 (BOPOP, BLE, NoLabel);
         else
            CodeGen1 (BOPOP, BPLUS, NoLabel);
         DecrementFrameSize();
         break;

      case ORNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BOR, NoLabel);
         DecrementFrameSize();
         break;

      case ANDNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BAND, NoLabel);
         DecrementFrameSize();
         break;

      case PRODNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BMULT, NoLabel);
         DecrementFrameSize();
         break;

      case DIVNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BDIV, NoLabel);
         DecrementFrameSize();
         break;

      case MODNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BMOD, NoLabel);
         DecrementFrameSize();
         break;

      case POWNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BEXP, NoLabel);
         DecrementFrameSize();
         break;

      case GENode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BGE, NoLabel);
         DecrementFrameSize();
         break;

      case EQNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BEQ, NoLabel);
         DecrementFrameSize();
         break;

      case NENode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BNE, NoLabel);
         DecrementFrameSize();
         break;

      case LTNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BLT, NoLabel);
         DecrementFrameSize();
         break;

      case GTNode :
         Expression ( Child(T,1) , CurrLabel);
         Expression ( Child(T,2) , NoLabel);
         CodeGen1 (BOPOP, BGT, NoLabel);
         DecrementFrameSize();
         break;

      case UMinusNode:
      case MinusNode :
         Expression ( Child(T,1) , CurrLabel);
         if (Rank(T) == 2)
         {
            Expression ( Child(T,2) , NoLabel);
            CodeGen1 (BOPOP, BMINUS, NoLabel);
            DecrementFrameSize();
         }
         else
            CodeGen1 (UOPOP, UNEG, NoLabel);
         break;

      case NOTNode :
         Expression ( Child(T,1) , CurrLabel);
         CodeGen1 (UOPOP, UNOT, NoLabel);
         DecrementFrameSize();
         break;

      case EOFNode :
         CodeGen1 (SOSOP, OSEOF, CurrLabel);
         IncrementFrameSize();
         break;

      case TRUENode :
         CodeGen1 (LITOP,MakeStringOf(1),CurrLabel);
         IncrementFrameSize();
         break;

      case FALSENode :
         CodeGen1 (LITOP,MakeStringOf(0),CurrLabel);
         IncrementFrameSize();
         break;

      case IntegerNode :
         CodeGen1 (LITOP, NodeName (Child(T,1)), CurrLabel);
         IncrementFrameSize();
         break;

     case CharacterNode :
         CodeGen1 (LITOP, MakeStringOf(Character(NodeName(Child(T,1)),1)), CurrLabel);
         IncrementFrameSize();
         break;
      
      case IdentifierNode :
         Reference (T,RightMode,CurrLabel);
         break;

      case PredNode:
         intrinsicFunction(T,CurrLabel,UPRED);
         break;

      case SuccNode:
         intrinsicFunction(T,CurrLabel,USUCC);
         break;

      case OrdNode:
         Expression(Child(T,1),CurrLabel);
         break;

      case ChrNode:
         Expression(Child(T,1),CurrLabel);
         break;

      default :
         ReportTreeErrorAt(T);
         printf ("<<< CODE GENERATOR >>> : UNKNOWN NODE NAME ");
         Write_String (stdout,NodeName(T));
         printf ("\n");

   } /* end switch */
} /* end Expression */

Clabel ProcessNode(TreeNode T, Clabel CurrLabel)
{
   int Kid, Num, Mode, Type, i;
   Clabel Label1, Label2, Label3;
   Clabel TempLabel;

   if (TraceSpecified)
   {
      fprintf (TraceFile, "<<< CODE GENERATOR >>> Processing Node ");
      Write_String (TraceFile, NodeName (T) );
      fprintf (TraceFile, " , Label is  ");
      Write_String (TraceFile, CurrLabel);
      fprintf (TraceFile, "\n");
   }
   switch (NodeName(T))
   {
      case ProgramNode :
         for(Kid = 2; Kid <= NKids(T)-1; Kid++){
            CurrLabel = ProcessNode(Child(T,Kid),CurrLabel);
         }
         return CurrLabel;

      case TypesNode :
         for (Kid = 1; Kid <= NKids(T); Kid++)
            CurrLabel = ProcessNode (Child(T,Kid), CurrLabel);
         return (CurrLabel);

      case TypeNode :
         if(NKids(T) > 1){
             if(NodeName(Child(T,2)) == LitNode){
                 for(Kid = 1; Kid <= NKids(Child(T,2)); Kid++){
                     Decorate(Child(Child(T,2),Kid),Kid-1);
                 }
             }
         }
         return CurrLabel;

      case ConstsNode:
      case DclnsNode :
         for (Kid = 1; Kid <= NKids(T); Kid++)
            CurrLabel = ProcessNode (Child(T,Kid), CurrLabel);
         if (NKids(T) > 0)
            return (NoLabel);
         else
            return (CurrLabel);

      case DclnNode :
         for (Kid = 1; Kid < NKids(T); Kid++){
            if (Kid != 1)
               CodeGen1 (LITOP,MakeStringOf(0),NoLabel);
            else
               CodeGen1 (LITOP,MakeStringOf(0),CurrLabel);
            Num = MakeAddress();
            Decorate ( Child(T,Kid), Num);
            IncrementFrameSize();
         }
         return (NoLabel);                 

      case ConstNode :
        if(NodeName(Child(T,2)) == IdentifierNode){
          Mode = Decoration(Child(Decoration(Child(T,2)),1));
          if(NodeName(Mode) == ConstNode){
              Decorate(Child(T,1),Decoration(Decoration(Child(T,2))));
              if(NodeName(Child(Mode,2)) == IntegerNode || Decoration(Child(Mode,2)) == TypeInteger){
                  Decorate(Child(T,2), TypeInteger);
              }else if(NodeName(Child(Mode,2)) == CharacterNode || Decoration(Child(Mode,2)) == TypeCharacter){
                  Decorate(Child(T,2), TypeCharacter);
              }
              else{
                  printf("unknown type for a const\n");
              }
          }else if(NodeName(Mode) == LitNode){
              Decorate(Child(T,1),MakeStringOf(Decoration(Decoration(Child(T,2)))));
              Decorate(Child(T,2), TypeInteger);
          }else{
              printf("unknown node type in const node decoration\n");
          }
        /* tipo char */
        }else if(NodeName(Child(T,2)) == CharacterNode){
          /* decoraciones> ver GRAN diagrama */
          Decorate(Child(T,1),MakeStringOf(Character(NodeName(Child(Child(T,2),1)),1)));
          Decorate(Child(T,2),TypeCharacter);
        /* tipo integer */
        }else if(NodeName(Child(T,2)) == IntegerNode){
          /* decoraciones> ver GRAN diagrama */
          Decorate(Child(T,1),NodeName(Child(Child(T,2),1)));
          Decorate(Child(T,2),TypeInteger);
        }
        return NoLabel;

      case BlockNode :
        for (Kid = 1; Kid <= NKids(T); Kid++)
           CurrLabel = ProcessNode (Child(T,Kid), CurrLabel);
        return (CurrLabel); 

      case AssignNode :
         Expression (Child(T,2), CurrLabel);
         Reference (Child(T,1), LeftMode, NoLabel);
         return (NoLabel);

      case OutputNode :
         for (Kid = 1; Kid <= NKids(T); Kid++){
               if(Kid > 1){
                 CurrLabel = NoLabel;
               }
               /* output string */
               if(NodeName(Child(T,Kid)) == StringNode){
                 int n =2;
                 while(Character(NodeName(Child(Child(T,Kid),1)),n) != '"'){
                   if(n > 2)
                     CurrLabel = NoLabel;
                   if(Character(NodeName(Child(Child(T,Kid),1)),n) == '\\' && Character(NodeName(Child(Child(T,Kid),1)),n+1)!= '"' ){
                     specialKeys(Character(NodeName(Child(Child(T,Kid),1)),n+1), CurrLabel);
                     IncrementFrameSize();
                     n++;
                   }else{
                     CodeGen1 (LITOP, MakeStringOf(Character(NodeName(Child(Child(T,Kid),1)),n)), CurrLabel);	  
                     IncrementFrameSize();
                   }
                   /* Output char por char */
                   CodeGen1 (SOSOP, OSOUTPUTC, NoLabel);
                   DecrementFrameSize();
                   n++;
                 }
                 IncrementFrameSize();
               }else{
                  Expression(Child(T,Kid),CurrLabel);
                  if(NodeName(Child(T,Kid)) == CharacterNode)
                    CodeGen1(SOSOP, OSOUTPUTC, NoLabel);
                  else if(NodeName(Child(T,Kid)) == IntegerNode)
                    CodeGen1(SOSOP, OSOUTPUT, NoLabel);
                  else if(NodeName(Child(T,Kid)) == IdentifierNode){
                    if(NodeName(Decoration(Child(Decoration(Child(T, Kid)),1))) == ConstNode){
                      Type = Decoration(Child(Decoration(Child(Decoration(Child(T, Kid)),1)),2));
                      /* output integer */
                      if(Type == TypeInteger)
                        CodeGen1 (SOSOP, OSOUTPUT, NoLabel);
                      /* output char */
                      else if(Type == TypeCharacter)
                        CodeGen1(SOSOP, OSOUTPUTC, NoLabel);
                      else
                        printf("Unknown type(decoration) for an input const");
                    }else if(NodeName(Decoration(Child(Decoration(Child(T, Kid)),1))) == DclnNode){
                      TreeNode dclnTreeNode = Decoration(Child(T, Kid));
                      output(dclnTreeNode, NoLabel);
                    }else{
                      printf("ERROR>>> cannot output unknown value type\n");
                    }
                  /* output char */
                  }else if(Decoration(Child(T, Kid)) == TypeCharacter){
                    CodeGen1 (SOSOP, OSOUTPUTC, NoLabel);
                  /* output integer */
                  }else if(Decoration(Child(T, Kid)) == TypeInteger){
                    CodeGen1 (SOSOP, OSOUTPUT, NoLabel);
                  }
                  /* output anything else */
                  else{
                    CodeGen1 (SOSOP, OSOUTPUT, NoLabel);
                  }
               }
               DecrementFrameSize();           
         }
         CodeGen1 (SOSOP, OSOUTPUTL, NoLabel);
         return (NoLabel);

      case IfNode :
         Expression (Child(T,1), CurrLabel);
         Label1 = MakeLabel();
         Label2 = MakeLabel();
         Label3 = MakeLabel();
         CodeGen2 (CONDOP,Label1,Label2, NoLabel);
         DecrementFrameSize();
         CodeGen1 (GOTOOP, Label3, ProcessNode (Child(T,2), Label1) );
         if (Rank(T) == 3)
            CodeGen0 (NOP, ProcessNode (Child(T,3),Label2));
         else
            CodeGen0 (NOP, Label2);
         return (Label3);

      case WhileNode :
         if (CurrLabel == NoLabel) 
            Label1 = MakeLabel();
         else 
            Label1 = CurrLabel;
         Label2 = MakeLabel();
         Label3 = MakeLabel();
         Expression (Child(T,1), Label1);
         CodeGen2 (CONDOP, Label2, Label3, NoLabel);
         DecrementFrameSize();
         CodeGen1 (GOTOOP, Label1, ProcessNode (Child(T,2), Label2) );
         return (Label3);


      case RepeatNode:
         if(CurrLabel == NoLabel){
            Label2 = MakeLabel();
            CurrLabel = Label2;
         }else{
            Label2 = CurrLabel;
         }
         for(Kid = 1; Kid <= NKids(T)-1; Kid++){
            CurrLabel = ProcessNode(Child(T,Kid), CurrLabel);
         }
         Expression(Child(T,NKids(T)), CurrLabel);
         Label1 = MakeLabel();
         CodeGen2(CONDOP, Label1, Label2, NoLabel);
         DecrementFrameSize();
         return Label1;

      case LoopNode:
          if(CurrLabel == NoLabel){
            Label2 = MakeLabel();
            CurrLabel = Label2;
          }else{
            Label2 = CurrLabel;
          }
          Label1 = MakeLabel();
          Decorate(T,Label1);
          for(Kid = 1; Kid <= NKids(T); Kid++){
            CurrLabel = ProcessNode(Child(T,Kid), CurrLabel);
          }
          CodeGen1(GOTOOP, Label2, CurrLabel);
          CodeGen1(GOTOOP, Label1, NoLabel);
          return Label1;

      case ExitNode:
          Label1 = Decoration(Decoration(T));
          CodeGen1(GOTOOP, Label1, CurrLabel);
          return NoLabel;

      case SwapNode:
         Reference(Child(T,1), RightMode, CurrLabel);
         Reference(Child(T,2), RightMode, NoLabel);
         CodeGen0(SWAPOP,NoLabel);
         Reference(Child(T,2), LeftMode, NoLabel);
         Reference(Child(T,1), LeftMode, NoLabel);
         return NoLabel;

      case UptoNode:
         /* asignar la variable del ciclo */
         Expression(Child(T,3),CurrLabel);
         Expression(Child(T,2),NoLabel);
         Reference(Child(T,1),LeftMode,NoLabel);
         /* inicializar etiquetas */
         Label1 = MakeLabel();
         Label2 = MakeLabel();
         Label3 = MakeLabel();
         CodeGen0(DUPOP, Label1);
         IncrementFrameSize();
         Reference(Child(T,1),RightMode,NoLabel);
         /* comparación de las variables del ciclo */
         CodeGen1(BOPOP,BGE,NoLabel);
         DecrementFrameSize();
         /* condición de salida del ciclo for*/
         CodeGen2(CONDOP,Label2,Label3,NoLabel);
         DecrementFrameSize();
         /* se procesan las instrucciones */
         CurrLabel = ProcessNode(Child(T,4), Label2);
         /* generación de código */
         Reference(Child(T,1),RightMode,CurrLabel);
         CodeGen1(UOPOP,USUCC,NoLabel);
         Reference(Child(T,1),LeftMode,NoLabel);
         CodeGen1(GOTOOP,Label1,NoLabel);
         CodeGen1(POPOP,MakeStringOf(1), Label3);
         CodeGen1(LITOP,MakeStringOf(0),NoLabel);
         Reference(Child(T,1),LeftMode,NoLabel);
         return NoLabel;

     case DownToNode:
         /* asignación de la variable del ciclo */
         Expression(Child(T,3),CurrLabel);
         Expression(Child(T,2),NoLabel);
         Reference(Child(T,1),LeftMode,NoLabel);
         /* inicializar etiquetas */
         Label1 = MakeLabel();
         Label2 = MakeLabel();
         Label3 = MakeLabel();
         CodeGen0(DUPOP, Label1);
         IncrementFrameSize();
         Reference(Child(T,1),RightMode,NoLabel);
         /* comparación de las variables del ciclo */
         CodeGen1(BOPOP,BLE,NoLabel);
         DecrementFrameSize();
         /* condición de salida del ciclo */
         CodeGen2(CONDOP,Label2,Label3,NoLabel);
         DecrementFrameSize();
         /* procesar instruciones*/
         CurrLabel = ProcessNode(Child(T,4), Label2);
         /* generación de código */
         Reference(Child(T,1),RightMode,CurrLabel);
         CodeGen1(UOPOP,UPRED,NoLabel);
         Reference(Child(T,1),LeftMode,NoLabel);
         CodeGen1(GOTOOP,Label1,NoLabel);
         CodeGen1(POPOP,MakeStringOf(1), Label3);
         CodeGen1(LITOP,MakeStringOf(0),NoLabel);
         Reference(Child(T,1),LeftMode,NoLabel);
         return NoLabel;

      case CaseNode:
         /* case evaluation */
         Expression(Child(T,1),CurrLabel);
         TempLabel = MakeLabel();
         Label3 = NoLabel;
         for(Kid = 2; Kid <= NKids(T); Kid++){
            /* mientras no sea la clausula otherwise */
            if(NodeName(Child(T,Kid)) != OtherwiseNode){
                /* duplicar la definición de datos */ 
                CodeGen0(DUPOP,Label3);
                IncrementFrameSize();
                /* si es una clausula de un entero */
                if(NodeName(Child(Child(T,Kid),1)) == IntegerNode){
                    CodeGen1(LITOP, NodeName(Child(Child(Child(T, Kid),1),1)), NoLabel);
                    IncrementFrameSize();
                    CodeGen1(BOPOP,BEQ,NoLabel);
                    DecrementFrameSize();
                }
                /* si es una clausula de un rango de enteros */
                else if(NodeName(Child(Child(T,Kid),1)) == RangeNode ){
                    CodeGen0(DUPOP,NoLabel);
                    IncrementFrameSize();
                    Expression(Child(Child(Child(T, Kid),1),1),NoLabel);
                    CodeGen1(BOPOP,BGE,NoLabel);
                    DecrementFrameSize();
                    CodeGen0(SWAPOP,NoLabel);
                    Expression(Child(Child(Child(T, Kid),1),2),NoLabel);
                    CodeGen1(BOPOP,BLE,NoLabel);
                    DecrementFrameSize();
                    CodeGen1(BOPOP,BAND,NoLabel);
                    DecrementFrameSize();
                }
                /* condición de igualdad */
                else{
                    Expression(Child(Child(T,Kid),1),NoLabel);
                    CodeGen1(BOPOP,BEQ,NoLabel);
                    DecrementFrameSize();
                }
                /* inicializar etiquetas(con solo 2 basta) */
                /* Una de salto y la de retorno */
                Label1 = MakeLabel();
                Label2 = MakeLabel();
                /* generación de código */
                CodeGen2(CONDOP,Label1,Label2,NoLabel);
                /* IncrementFrameSize(); */
                DecrementFrameSize();
                CodeGen1(POPOP, MakeStringOf(1),Label1);
                CurrLabel = ProcessNode(Child(Child(T, Kid), 2),NoLabel);
                CodeGen1(GOTOOP,TempLabel,CurrLabel);
                Label3 = Label2; /* para la siguiente clausula */
            }
         }
         /* final del 'switch' */
         CodeGen1(POPOP,MakeStringOf(1),Label2);
         DecrementFrameSize();
         /* condición por defecto del switch */
         if( NodeName(Child(T,NKids(T))) == OtherwiseNode ){
            CurrLabel = ProcessNode(Child(Child(T, NKids(T)),1), NoLabel);
            if(CurrLabel == NoLabel){
                /* retorno */
                CodeGen0(NOP, CurrLabel);
            }
         }
         return TempLabel;

      case ReadNode :
         for(Kid = 1; Kid <= NKids(T); Kid++){
            if(Kid != 1){
                CurrLabel = NoLabel;
            }
            input(Decoration(Child(T, Kid)), CurrLabel);
            Reference (Child(T,Kid), LeftMode, NoLabel);
        }
        return NoLabel;

      case NullNode : return(CurrLabel);

      default :
        ReportTreeErrorAt(T);
        printf ("<<< CODE GENERATOR >>> : UNKNOWN NODE NAME ");
        Write_String (stdout,NodeName(T));
        printf ("\n");

   } /* end switch */
};   /* end ProcessNode */

/* funciones intrinsecas para valores enteros, caracteres y enumerados */
void intrinsicFunction(TreeNode T, Clabel CurrLabel, int UOP){
    Expression(Child(T,1),CurrLabel);
    CodeGen1(UOPOP,UOP,NoLabel);
    if(NodeName(Decoration(T)) == TypeNode && NKids(Decoration(T)) > 1 && NodeName(Child(Decoration(T),2)) == LitNode){
        TreeNode LitTreeNode = Child(Decoration(T),2);
        CodeGen1(LITOP,MakeStringOf(0),NoLabel);
        CodeGen1(LITOP,MakeStringOf(Decoration(Child(LitTreeNode,NKids(LitTreeNode)))),NoLabel);
        CodeGen0(LIMITOP,NoLabel);
    }
}

/* procesar teclas especiales> salto de línea(10), tabulador(9) y backspace(8) */
void specialKeys(int ascii, Clabel CurrLabel){
    int res = -1;

    switch(ascii){
        case 'n': res = 10; break;      /* newline */
        case 't': res = 9;  break;      /* tab */
        case 'b': res = 8;  break;      /* backspace */
        default: printf("ERROR>>> UNRECOGNIZED SPECIAL KEY"); break;
    }
    CodeGen1(LITOP,MakeStringOf(res),CurrLabel);
}

void input(TreeNode T, Clabel CurrLabel){
    /* input integer */
    if(Decoration(Decoration(Child(Decoration(Child(T,1)),NKids(Decoration(Child(T,1)))))) == TypeInteger){
        CodeGen1(SOSOP,OSINPUT,CurrLabel);
    /* input character */
    }else if(Decoration(Decoration(Child(Decoration(Child(T,1)),NKids(Decoration(Child(T,1)))))) == TypeCharacter){
        CodeGen1(SOSOP,OSINPUTC,CurrLabel);
    /* excepción: debe ser entero o caracter */
    }else{
        CodeGen1(SOSOP,OSINPUT,CurrLabel);
        printf("<<<CODE GENERATOR>>>: cannot read values different from integer and char\n\n");
    }
    IncrementFrameSize();
}

void output(TreeNode T, Clabel CurrLabel){
  
  if(Decoration(Decoration(Child(Decoration(Child(T,1)), NKids(Decoration(Child(T,1)))))) == TypeInteger){
    CodeGen1 (SOSOP, OSOUTPUT, CurrLabel);
  }
  else if(Decoration(Decoration(Child(Decoration(Child(T,1)), NKids(Decoration(Child(T,1)))))) == TypeCharacter){
    CodeGen1 (SOSOP, OSOUTPUTC, CurrLabel);
  }
  else{
    printf("<<<CODE GENERATOR>>>: cannot output values different from integer and char\n\n");
  }
}
