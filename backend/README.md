
MutatorList = [

    "PLUS",
    "MINUS",
    "MULT",
    "MULT_HIGHPART",
    "RDIV",
    "TRUNC_DIV",
    "FLOOR_DIV",
    "CEIL_DIV",
    "ROUND_DIV",
    "TRUNC_MOD",
    "FLOOR_MOD",
    "CEIL_MOD",
    "ROUND_MOD",
    "EXACT_DIV",
    
    "ABS",
    "NEGATE",

    "TRUTH_AND",
    "TRUTH_ANDIF",
    "TRUTH_NOT",
    "TRUTH_OR",
    "TRUTH_ORIF",
    "TRUTH_XOR",

    "BIT_AND",
    "BIT_ANDTC",
    "BIT_IOR",
    "BIT_NOT",
    "BIT_XOR",
    "LROTATE",
    "LSHIFT",
    "RROTATE",
    "RSHIFT",

    "EQ",
    "GE",
    "GT",
    "LE",
    "LT",
    "NE",
    "ORDERED",
    "UNEQ",
    "UNGE",
    "UNGT",
    "UNLE",
    "UNLT",
    "UNORDERED",

    "CONST_TO_CONST",
    "INSERT_CONST",
    "VAR_TO_CONST"
]

make指令需要指定参数： 

make INTERFACE=<1> FROM=<2> TO=<3>

每个参数都可以缺省，缺省的默认值在makefile开头中有。

注意变异算子接口和变异算子需要一致

使用例子如下：

make INTERFACE=AOR FROM=PLUS TO=MINUS

make FROM=PLUS TO=MINUS

make FROM=MULT TO=RDIV

目前可以接受三种参数指定：  
    1.start_line 开始行号  
    2.end_line 结束行号  
    3.var_name 要进行改变的变量  
    .help = "-fplugin-arg-plugin-start_line=<num>"  
            "-fplugin-arg-plugin-end_line=<num>"  
            "-fplugin-arg-plugin-var_name=<name>"  