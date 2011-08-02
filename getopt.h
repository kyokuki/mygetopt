/* Declarations for getopt.
   Copyright (C) 1989-1994, 1996-1999, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _GETOPT_H


#ifdef	__cplusplus
extern "C" {
#endif

/* For communication from `getopt' to the caller.
   When `getopt' finds an option that takes an argument,
   the argument value is returned here.
   Also, when `ordering' is RETURN_IN_ORDER,
   each non-option ARGV-element is returned here.  */

/* 当'getopt'找到一个带有参数的选项时,那个参数返回到'optarg'.
 * 当'ordering'的值是'RETURN_IN_ORDER'时,每一个非选项参数
 * 返回到这里 */
    
    extern char *optarg;

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to `getopt'.

   On entry to `getopt', zero means this is the first call; initialize.

   When `getopt' returns -1, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, `optind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

/* 下一个将要被扫描探测的'ARGV'的索引. */
    
    extern int optind;

/* Callers store zero here to inhibit the error message `getopt' prints
   for unrecognized options.  */

/* 探测到不识别的选项使,是否要输出错误信息.
 * 0 - 不输出
 * 1 - 输出
 * */

    extern int opterr;

/* Set to an option character which was unrecognized.  */

/* 设置为探测到的不识别的选项字符 */
    extern int optopt;

/* Describe the long-named options requested by the application.
   The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
   of `struct option' terminated by an element containing a name which is
   zero.

   The field `has_arg' is:
   no_argument		(or 0) if the option does not take an argument,
   required_argument	(or 1) if the option requires an argument,
   optional_argument 	(or 2) if the option takes an optional argument.

   If the field `flag' is not NULL, it points to a variable that is set
   to the value given in the field `val' when the option is found, but
   left unchanged if the option is not found.

   To have a long-named option do something other than set an `int' to
   a compiled-in constant, such as set a value from `optarg', set the
   option's `flag' field to zero and its `val' field to a nonzero
   value (the equivalent single-letter option character, if there is
   one).  For long options that have a zero `flag' field, `getopt'
   returns the contents of the `val' field.  */

    struct option {
	const char *name;
	//char *name;
	/* has_arg can't be an enum because some compilers complain about
	   type mismatches in all the code that assumes it is an int.  */
	int has_arg;
	int *flag;
	int val;
    };

/* Names for the values of the `has_arg' field of `struct option'.  */
/* ‘struct option’的'has_arg'字段使用的值.分别为:
 * 无参数       0
 * 需要参数       1
 * 任选的参数   2
 * */
# define no_argument		0
# define required_argument	1
# define optional_argument	2


    extern int getopt (int ___argc, char *const *___argv, 
                const char *__shortopts);

    extern int getopt_long(int ___argc, char *const *___argv,
                const char *__shortopts,
                const struct option *__longopts,
                int *__longind);

/* Internal only.  Users should not call this directly.  */
    extern int _getopt_internal(int ___argc, char *const *___argv,
				const char *__shortopts,
				const struct option *__longopts,
				int *__longind, int __long_only);

#ifdef	__cplusplus
}
#endif

#endif				/* getopt.h */
