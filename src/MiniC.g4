grammar MiniC;

prog: (decl)+ EOF
	;

decl: variableDecl #declVariableDecl
	| functionDecl #declFunctionDecl
	;

variableDecl: variable ';' #variableDeclVariable
			| TYPE identifier '=' expr ';' #variableDeclAssignment
			;

identifier: ID( '[' expr ']' )? ( '[' expr ']' )?
		  ;

variable: TYPE identifier
		;

functionDecl: TYPE ID '(' params? ')' '{' statementBlock '}'
			;

params: variable ( ',' variable )*
	  ;

statementBlock: statement statementBlock #statementBlockStatement
			  | variableDecl statementBlock #statementBlockVariableDecl
			  |                             #statementBlockEmpty
			  ;

statement: assignStatement ';' #statementAssignStatement
		 | forStatement #statementForStatement
		 | whileStatement #statementWhileStatement
		 | ifStatement #statementIfStatement
		 | ifElseStatement #statementIfElseStatement
		 | breakStatement #statementBreakStatement
		 | continueStatement #statementContinueStatement
		 | returnStatement #statementReturnStatement
		 | print ';' #statementPrint
		 | readString ';' #statementReadString
		 | call ';' #statementCallStatement
		 ;

print: 'print' '(' callParams? ')'
	 ;

readInt: 'readInt' '(' ')'
	   ;

readChar: 'readChar' '(' ')'
		;

readString: 'readString' '(' identifier ')'
          ; 

assignStatement: identifier '=' expr #assignStatementNormal
	  		   | identifier UNARYOP #assignStatementUnary
	  		   ;

forStatement: FOR '(' assignStatement ';' expr ';' assignStatement ')' '{' statementBlock '}'
   			;

whileStatement: WHILE '(' expr ')' '{' statementBlock '}'
	 		  ;

ifStatement: IF '(' expr ')' '{' statementBlock '}'
  		   ;

ifElseStatement: IF '(' expr ')' '{' statementBlock '}' ELSE '{' statementBlock '}'
	   		   ;

breakStatement: BREAK ';'
	 		  ;

continueStatement: CONTINUE ';'
				 ;

returnStatement: RETURN expr ';'
	  		   ;

expr: '(' expr ')' #exprParentheses
	| identifier UNARYOP #exprUnaryOP
	| '-' expr #exprNegate
	| '!' expr #exprNot
	| '(' TYPE ')' expr #exprCast
	| expr op=('*'|'/'|'%') expr #exprMulDivMod
	| expr op=('+'|'-') expr #exprAddSub
	| expr op=('>'|'<'|'>='|'<=') expr #exprLtGt
	| expr op=('=='|'!=') expr #exprEquals
	| expr '&&' expr #exprBoolAnd
	| expr '||' expr  #exprBoolOr
	| expr '?' expr ':' expr #exprTernary
	| readInt #exprReadInt
	| readChar #exprReadChar
	| call #exprCall
	| CONSTANT #exprConstant
	| identifier #exprIdentifier
	;

call: ID '(' callParams? ')'
	;

callParams: expr ( ',' expr )* 
		  ;


/*Tokens*/
TYPE: 'int'
	| 'uint'
	| 'char'
	| 'bool'
	| 'void'
	;

FOR: 'for'
   ;

WHILE: 'while'
	 ;

IF: 'if'
  ;

ELSE: 'else'
	;

BREAK: 'break'
	 ;

CONTINUE: 'continue'
		;

RETURN: 'return'
	  ;

CONSTANT: 'true'
		| 'false'
		| [0-9][0-9]*
		| ['][ -~][']
		| [']'\\n'[']
		| [']'\\t'[']
		| [']'\\0'[']
		| 'EOF'
		;

ID: [a-zA-Z][a-zA-Z0-9_]*
  ;

UNARYOP: '++'
	   | '--'
	   ;

COMMENT : '#' ~[\r\n]*->skip; 
NS : [ \t\n]+ -> skip; 