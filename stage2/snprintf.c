/****************************************************************************

  Copyright (c) 1999,2000 WU-FTPD Development Group.
  All rights reserved.

  Portions Copyright (c) 1980, 1985, 1988, 1989, 1990, 1991, 1993, 1994
    The Regents of the University of California.
  Portions Copyright (c) 1993, 1994 Washington University in Saint Louis.
  Portions Copyright (c) 1996, 1998 Berkeley Software Design, Inc.
  Portions Copyright (c) 1989 Massachusetts Institute of Technology.
  Portions Copyright (c) 1998 Sendmail, Inc.
  Portions Copyright (c) 1983, 1995, 1996, 1997 Eric P.  Allman.
  Portions Copyright (c) 1997 by Stan Barber.
  Portions Copyright (c) 1997 by Kent Landfield.
  Portions Copyright (c) 1991, 1992, 1993, 1994, 1995, 1996, 1997
    Free Software Foundation, Inc.

  Use and distribution of this software and its source code are governed
  by the terms and conditions of the WU-FTPD Software License ("LICENSE").

  If you did not receive a copy of the license, it may be obtained online
  at http://www.wu-ftpd.org/license.html.

  $Id: snprintf.c,v 1.1 2001/07/06 19:23:39 scut Exp $

****************************************************************************/

#ifndef __P
#define __P(p)  p
#endif

#include <stdarg.h>
#define VA_LOCAL_DECL	va_list ap;
#define VA_START(f)	va_start(ap, f)
#define VA_END		va_end(ap)

#ifdef SOLARIS2
#ifdef _FILE_OFFSET_BITS
#define SOLARIS26
#endif
#endif

#ifdef SOLARIS26
#define HAS_SNPRINTF
#define HAS_VSNPRINTF
#endif
#ifdef _SCO_DS_
#define HAS_SNPRINTF
#endif
#ifdef luna2
#define HAS_VSNPRINTF
#endif
/*
   **  SNPRINTF, VSNPRINT -- counted versions of printf
   **
   **   These versions have been grabbed off the net.  They have been
   **   cleaned up to compile properly and support for .precision and
   **   %lx has been added.
 */

/**************************************************************
 * Original:
 * Patrick Powell Tue Apr 11 09:48:21 PDT 1995
 * A bombproof version of doprnt (dopr) included.
 * Sigh.  This sort of thing is always nasty do deal with.  Note that
 * the version here does not include floating point...
 *
 * snprintf() is used instead of sprintf() as it does limit checks
 * for string length.  This covers a nasty loophole.
 *
 * The other functions are there to prevent NULL pointers from
 * causing nast effects.
 **************************************************************/

/*static char _id[] = "$Id: snprintf.c,v 1.1 2001/07/06 19:23:39 scut Exp $"; */
static void dopr(char *, const char *, va_list);
static char *end;

#ifndef HAS_VSNPRINTF
int vsnprintf(char *str, int count, const char *fmt, va_list args)
{
    int	n = 0;
    str[0] = 0;
    end = str + count - 1;
    dopr(str, fmt, args);
    if (count > 0)
	end[0] = 0;
    while (*str++)
	    n++;
    return n;
}

#ifndef HAS_SNPRINTF
/* VARARGS3 */
int snprintf(char *str, int count, const char *fmt,...)
{
    int len;
    VA_LOCAL_DECL

	VA_START(fmt);
    len = vsnprintf(str, count, fmt, ap);
    VA_END;
    return len;
}
#endif

/*
 * dopr(): poor man's version of doprintf
 */

static void fmtstr __P((char *value, int ljust, int len, int zpad, int maxwidth));
static void fmtnum __P((long value, int base, int dosign, int ljust, int len, int zpad));
static void dostr __P((char *, int));
static char *output;
static void dopr_outch __P((int c));

static void dopr(char *buffer, const char *format, va_list args)
{
    int ch;
    long value;
    int longflag = 0;
    int pointflag = 0;
    int maxwidth = 0;
    char *strvalue;
    int ljust;
    int len;
    int zpad;

    output = buffer;
    while ((ch = *format++)) {
	switch (ch) {
	case '%':
	    ljust = len = zpad = maxwidth = 0;
	    longflag = pointflag = 0;
	  nextch:
	    ch = *format++;
	    switch (ch) {
	    case 0:
		dostr("**end of format**", 0);
		return;
	    case '-':
		ljust = 1;
		goto nextch;
	    case '0':		/* set zero padding if len not set */
		if (len == 0 && !pointflag)
		    zpad = '0';
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		if (pointflag)
		    maxwidth = maxwidth * 10 + ch - '0';
		else
		    len = len * 10 + ch - '0';
		goto nextch;
	    case '*':
		if (pointflag)
		    maxwidth = va_arg(args, int);
		else
		    len = va_arg(args, int);
		goto nextch;
	    case '.':
		pointflag = 1;
		goto nextch;
	    case 'l':
		longflag = 1;
		goto nextch;
	    case 'u':
	    case 'U':
		/*fmtnum(value,base,dosign,ljust,len,zpad) */
		if (longflag) {
		    value = va_arg(args, long);
		}
		else {
		    value = va_arg(args, int);
		}
		fmtnum(value, 10, 0, ljust, len, zpad);
		break;
	    case 'o':
	    case 'O':
		/*fmtnum(value,base,dosign,ljust,len,zpad) */
		if (longflag) {
		    value = va_arg(args, long);
		}
		else {
		    value = va_arg(args, int);
		}
		fmtnum(value, 8, 0, ljust, len, zpad);
		break;
	    case 'd':
	    case 'D':
		if (longflag) {
		    value = va_arg(args, long);
		}
		else {
		    value = va_arg(args, int);
		}
		fmtnum(value, 10, 1, ljust, len, zpad);
		break;
	    case 'x':
		if (longflag) {
		    value = va_arg(args, long);
		}
		else {
		    value = va_arg(args, int);
		}
		fmtnum(value, 16, 0, ljust, len, zpad);
		break;
	    case 'X':
		if (longflag) {
		    value = va_arg(args, long);
		}
		else {
		    value = va_arg(args, int);
		}
		fmtnum(value, -16, 0, ljust, len, zpad);
		break;
	    case 's':
		strvalue = va_arg(args, char *);
		if (maxwidth > 0 || !pointflag) {
		    if (pointflag && len > maxwidth)
			len = maxwidth;		/* Adjust padding */
		    fmtstr(strvalue, ljust, len, zpad, maxwidth);
		}
		break;
	    case 'c':
		ch = va_arg(args, int);
		dopr_outch(ch);
		break;
	    case '%':
		dopr_outch(ch);
		continue;
	    default:
		dostr("???????", 0);
	    }
	    break;
	default:
	    dopr_outch(ch);
	    break;
	}
    }
    *output = 0;
}

static void fmtstr(char *value, int ljust, int len, int zpad, int maxwidth)
{
    int padlen, strlen;		/* amount to pad */

    if (value == 0) {
	value = "<NULL>";
    }
    for (strlen = 0; value[strlen]; ++strlen);	/* strlen */
    if (strlen > maxwidth && maxwidth)
	strlen = maxwidth;
    padlen = len - strlen;
    if (padlen < 0)
	padlen = 0;
    if (ljust)
	padlen = -padlen;
    while (padlen > 0) {
	dopr_outch(' ');
	--padlen;
    }
    dostr(value, maxwidth);
    while (padlen < 0) {
	dopr_outch(' ');
	++padlen;
    }
}

static void fmtnum(long value, int base, int dosign, int ljust, int len, int zpad)
{
    int signvalue = 0;
    unsigned long uvalue;
    char convert[20];
    int place = 0;
    int padlen = 0;		/* amount to pad */
    int caps = 0;

    /* DEBUGP(("value 0x%x, base %d, dosign %d, ljust %d, len %d, zpad %d\n",
       value, base, dosign, ljust, len, zpad )); */
    uvalue = value;
    if (dosign) {
	if (value < 0) {
	    signvalue = '-';
	    uvalue = -value;
	}
    }
    if (base < 0) {
	caps = 1;
	base = -base;
    }
    do {
	convert[place++] =
	    (caps ? "0123456789ABCDEF" : "0123456789abcdef")
	    [uvalue % (unsigned) base];
	uvalue = (uvalue / (unsigned) base);
    } while (uvalue);
    convert[place] = 0;
    padlen = len - place;
    if (padlen < 0)
	padlen = 0;
    if (ljust)
	padlen = -padlen;
    /* DEBUGP(( "str '%s', place %d, sign %c, padlen %d\n",
       convert,place,signvalue,padlen)); */
    if (zpad && padlen > 0) {
	if (signvalue) {
	    dopr_outch(signvalue);
	    --padlen;
	    signvalue = 0;
	}
	while (padlen > 0) {
	    dopr_outch(zpad);
	    --padlen;
	}
    }
    while (padlen > 0) {
	dopr_outch(' ');
	--padlen;
    }
    if (signvalue)
	dopr_outch(signvalue);
    while (place > 0)
	dopr_outch(convert[--place]);
    while (padlen < 0) {
	dopr_outch(' ');
	++padlen;
    }
}

static void dostr(char *str, int cut)
{
    if (cut) {
	while (*str && cut-- > 0)
	    dopr_outch(*str++);
    }
    else {
	while (*str)
	    dopr_outch(*str++);
    }
}

static void dopr_outch(int c)
{
#if 0
    if (iscntrl(c) && c != '\n' && c != '\t') {
	c = '@' + (c & 0x1F);
	if (end == 0 || output < end)
	    *output++ = '^';
    }
#endif
    if (end == 0 || output < end)
	*output++ = c;
}

#endif

