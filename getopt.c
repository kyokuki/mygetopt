/* 
 *      file : getopt.c
 *      desc : option parse program
 *
*       update:     
*       2011-07-30 12:00     short-option and long-option had been surported.
*
*       2011-08-02           short-option's additional argument has been surported.
*                            if scanned an unrecognize option or need an argument but not giving,
*                            then stop scan(by setting optind = argc).
 * */

#include <stdio.h>
#include "getopt.h"

/* 当'getopt'找到一个带有参数的选项时,那个参数返回到'optarg'.
 * 当'ordering'的值是'RETURN_IN_ORDER'时,每一个非选项参数
 * 返回到这里 */

char *optarg;

/* 1003.2 says this must be 1 before any call.  */
/* 下一个将要被扫描探测的'ARGV'的索引. */
int optind = 1;

int __getopt_initialized;

/* 处理像 ‘demo -abc’中的'-abc'选项,本次返回选项'a'，
   'nextchar'指向下一个字符以便下次处理'b'选项.
   
   处理像'demo --write=filename'中的'--write=filename'选项,
   'nextchar'指向参数名字'write'开头.
   
   处理像'demo -a -b -cdd'(optsting=abc::)中的'-cdd'选项,
   'nextchar'指向'dd',并作为该选项参数返回给'optarg'
 */
static char *nextchar;

/* 探测到不识别的选项使,是否要输出错误信息.
 * 0 - 不输出
 * 1 - 输出	*/

int opterr = 1;

/* 设置为探测到的不识别的选项字符 */

int optopt = '?';

static char *my_index(str, chr)
const char *str;
int chr;
{
	while (*str) {
		if (*str == chr)
			return (char *)str;
		str++;
	}
	return 0;
}

int _getopt_internal(argc, argv, optstring, longopts, longind, long_only)
int argc;
char *const *argv;
const char *optstring;
const struct option *longopts;
int *longind;
int long_only;
{
	int print_errors = opterr;
	/* 'optstring'以':'开头则表示不输出错误信息. */
	if (optstring[0] == ':') {
		print_errors = 0;
		++optstring;
	}
	if (argc < 1)
		return -1;

	if (optind >= argc)
		return -1;

	optarg = NULL;

	/* 初始化 */
	if (0 == optind || !__getopt_initialized) {
		if (0 == optind)
			optind = 1;	/*  不探测ARGV[0],即程序名 */

		nextchar = NULL;	/* nextchar必须初始化为NULL */
		__getopt_initialized = 1;
	}

	/* 假设'optsting'='abc:d::'
	 * 短选为'-a -b -c c -dd' 或 '-ab -c c'
	 * 长选项为'--list-all --count=512'
	 * 
	 * 为处理'-ab'此类多个选项在同一个'ARGV-element'中,
	 * 必须使用'nextchar'来定位下一个参数.
	 * 因此首先用'nextchar'来进行参数判断.  *
	 * */

	if (nextchar == NULL || *nextchar == '\0') {
		/* 此分支说明类似'-ab'或'--count=512'的'ARGV-element'的探测
		 * 扫描已经结束, 此次扫描是下一个'ARGV-element'
		 * */
		;		/* do nothing for now */
	}

	/* 处理长选项
	 * */
	if (longopts != NULL
	    && argv[optind][0] == '-' && argv[optind][1] == '-') {
		const struct option *p;
		const struct option *pfound = NULL;
		char *nameend = NULL;
		int indfound = -1;
		int option_index;

		/* 指向长选项名字结尾 */
		for (nextchar = nameend = argv[optind]; *nameend && *nameend != '='; nameend++) ;	/* just do nothing */

		nextchar += 2;	/* nextchar 指向长选型名字开始处 */

		/* 遍历长选项结构数组'longopts',判断该长选项是否有效 */
		for (p = longopts, option_index = 0; p->name;
		     p++, option_index++) {
			if (!strncmp(p->name, nextchar, (nameend - nextchar))) {
				if ((unsigned int)strlen(p->name)
				    == (unsigned int)(nameend - nextchar)) {
					/* 长选项完全匹配 */
					pfound = p;
					indfound = option_index;
					break;
				}
			}
		}

		/* 长选项有效 */
		if (pfound != NULL) {
			optind++;

			/* 命令行中该长选项后有参数 */
			if (*nameend) {
				/* 该长选项有参数 */
				if (pfound->has_arg)
					optarg = nameend + 1;
				else {
					if (1 == print_errors) {
						fprintf(stderr,
							"%s: option `--%s` doesn't allow an argument\n",
							argv[0], pfound->name);
					}

					/* 是否 遇到不识别的选项或缺少参数时终止扫描 */
					if (1) {
						optind = argc;
						return -1;
					}
					nextchar += strlen(nextchar);
					/* 命令行中有参数但实际不需参数:
					 * 设置'optop'
					 * 并返回'?'
					 * */
					optopt = pfound->val;
					nextchar += strlen(nextchar);
					return '?';	/* 返回值='?' ---> 无效选项 */

				}
			} else if (pfound->has_arg == 1) {	/* 命令行中该长选项无参数,但需要参数  */
				if (1 == print_errors) {
					fprintf(stderr,
						"%s: option `--%s` requires an argument\n",
						argv[0], pfound->name);
				}

				/* 是否 遇到不识别的选项或缺少参数时终止扫描 */
				if (1) {
					optind = argc;
					return -1;
				}
				nextchar += strlen(nextchar);

				optopt = pfound->val;
				return ':';	/* 返回值='：' ---> 缺少选项参数 */
			}

			/* ‘nextchr’指向结尾(即指向'\0') */
			nextchar += strlen(nextchar);

			if (longind != NULL)
				*longind = indfound;

			/* 不为NULL则使flag指向的变量填入val的值 */
			if (pfound->flag != NULL) {
				*(pfound->flag) = pfound->val;
				return 0;	/* 返回值=0 */
			}

			/* flag==NULL则返回'0'值 */
			return pfound->val;
		}
		/* 长选项有效 --end */
		if (print_errors) {
			char buf[50];
			int count = nameend - nextchar;
			strncpy(buf, nextchar, count);
			buf[count] = '\0';
			fprintf(stderr, "%s : invalid option --%s\n", argv[0],
				buf);
		}
		/* 是否 遇到不识别的选项或缺少参数时终止扫描 */
		if (1) {
			optind = argc;
			return -1;
		}

		optind++;
		nextchar += strlen(nextchar);
		return '?';	/* 无效选项 */
	}	/* 处理长选项 --end */
	/* 处理短选项
	 * optstring:
	 * 'c:'    --> 必带参数
	 * 'c::'   --> 可选参数
	 * */
	if (argv[optind][0] == '-') {
		char c;
		char *temp = NULL;

		if (nextchar == NULL || *nextchar == '\0') {
			nextchar = argv[optind] + 1;
		}

		c = *nextchar++;
		temp = my_index(optstring, c);

		/* 此次是处理'-abc'短选项的最后一个字符,那么'optind'加1 */
		if (*nextchar == '\0') {
			optind++;
		}

		if (temp == NULL || *temp == ':') {
			if (print_errors) {
				fprintf(stderr, "%s: invalid option -%c\n",
					argv[0], c);
			}

			/* 是否 遇到不识别的选项或缺少参数时终止扫描 */
			if (1) {
				optind = argc;
				return -1;
			}
			optind++;
			nextchar = NULL;
			optopt = c;
			return '?';
		}

		/* 处理'带参数的选项'以及'可选参数的选项' */
		if (temp[1] == ':') {
			if (temp[2] == ':') {
				/* 任选的选项 */
				/* 将该选项后的所有字符都当作该选项的参数,optind加一 */
				/* optstring='a::' , 命令行参数=' -abcd' 或 ' -a bcd' 或 '-a -e'  */
				if (*nextchar != '\0') {
					optarg = nextchar;
					nextchar += strlen(nextchar);
					optind++;
				}
				/* 如果该选项后面没有可用字符('-a bcd' 或 '-a -e'), 则optind已经指向下一个 option-element  */
				/* 设置返回参数为下一个option-element, optind再次加一 */
				else if (optind < argc) {
					if (argv[optind][0] != '-') {
						optarg = argv[optind];
						optind++;
					} else {
						/* 该选项没有参数 */
						optarg = NULL;
					}
				} else {
					/* 此时 optind == argc, 但参数可选, 没有错误  */
					;
				}

				//fprintf(stderr, "additional option argument is not supported yet !\n");
			} else {	/* 必须带参数的选项 */

				if (*nextchar != '\0') {
					/* 如果要用本次'ARGV-element'的剩下的字符作为该选项的参数
					 * 那么'optind'就要指向下一个'ARGV-element'
					 * */
					optarg = nextchar;
					optind++;
				} else if (optind == argc) {
					if (print_errors) {
						fprintf(stderr,
							"%s: option requires an argument -%c\n",
							argv[0], c);
					}

					/* 是否 遇到不识别的选项或缺少参数时终止扫描 */
					if (1) {
						optind = argc;
						return -1;
					}

					optopt = c;
					nextchar = NULL;
					c = ':';	/* 返回值':'  ---> 缺少选项参数 */
				} else {
					/* 已经在上面吧'optind'加1了 */
					optarg = argv[optind++];
				}

				nextchar = NULL;
			}
		}

		return c;

	}
	/* 处理短选项  ---> end */
	return -1;
}

int getopt(argc, argv, optstring)
int argc;
char *const *argv;
const char *optstring;
{
	return _getopt_internal(argc, argv, optstring,
				(const struct option *)0, (int *)0, 0);
}

int getopt_long(argc, argv, options, long_options, opt_index)
int argc;
char *const *argv;
const char *options;
const struct option *long_options;
int *opt_index;
{
	return _getopt_internal(argc, argv, options, long_options, opt_index,
				0);
}
