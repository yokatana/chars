#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define PROG_NAME	"chars"
#define PROG_VERSION	"0.1"

static const char* const usage =
"Usage: " PROG_NAME " [OPTIONS] [FILE]\n\n"
"Split files into characters, one per line or separated by spaces, with\n"
"additional formatting and escaping options.\n\n"
"FILE is used at the input file. When a hyphen (-) or omitted, standard\n"
"input is used.\n\n"
"General options:\n"
" -v, --version          print version number and exit\n"
" -h, --help             print this message and exit\n"
" -o, --output  <file>   path or - for standard output (default)\n"
" -p, --printable        skip control characters from the input\n"
" -s  --oneline          separate output by spaces, not newlines\n"
" -r  --raw              don't escape control characters\n"
" -q  --quote            qoute characters with \"\n"
"Output style options with printf quivalents:\n"
"                        ASCII characters (default)         %%c\n"
" -u, --decimal          ASCII codepoints                   %%3i\n"
" -x, --hex              ASCII codepoints in hex            %%02x\n"
" -X, --Hex              ASCII codepoints in uppercase hex  %%02X\n\n"
"Also see the man page: man chars\n";

struct fmt_entry {
	const char* short_opt;
	const char* long_opt;
	const char* fmt;
};

static const struct fmt_entry fmt_table[] = {
	{ "-u", "--decimal", "%3u" },
	{ "-x", "--hex", "%02x" },
	{ "-X", "--Hex", "%02X" }
};

static void missing_val(const char* optname)
{
	fprintf(stderr, PROG_NAME ": missing argument for %s\n", optname);
	exit(EXIT_FAILURE);
}

static const char* path_from_opt(const char* val)
{
	if (strcmp(val, "-") == 0) {
		return NULL;
	} else {
		return val;
	}
}

static const char* fmt_from_opt(const char* opt)
{
	int table_len = sizeof(fmt_table) / sizeof(fmt_table[0]);
	for (int i = 0; i < table_len; i++) {
		const char* s = fmt_table[i].short_opt;
		const char* l = fmt_table[i].long_opt;
		if ((s && strcmp(opt, s) == 0)
				|| (l && strcmp(l, opt) == 0)) {
			return fmt_table[i].fmt;
		}
	}

	return NULL;
}

static FILE* open_or_default(const char* path, const char* mode,
		FILE* fallback)
{
	if (!path) {
		return fallback;
	}


	FILE* file = fopen(path, mode);
	if (!file) {
		perror(PROG_NAME);
		exit(EXIT_FAILURE);
	}

	return file;
}

static const char* escape_char(int ch)
{
	static char buf[5];

	switch (ch) {
	case '\a': return "\\a";
	case '\b': return "\\b";
	case '\f': return "\\f";
	case '\n': return "\\n";
	case '\r': return "\\r";
	case '\t': return "\\t";
	case '\v': return "\\v";
	case '\'': return "\\'";
	case '\"': return "\\\"";
	case '\\': return "\\\\";

	default:
		snprintf(buf, sizeof(buf), "\\%o", ch & 0xFF);
		return buf;
	}
}

int main(int argc, const char* argv[])
{
	const char* fmt = "%c";
	const char* inpath = NULL;
	const char* outpath = NULL;
	bool only_printable = false;
	bool oneline = false;
	bool escape = true;
	bool quote = false;

	for (int i = 1; i < argc; i++) {
		const char* arg = argv[i];
		const char* new_fmt;
		if (strcmp("-v", arg) == 0 ||
				strcmp("--version", arg) == 0) {
			printf(PROG_VERSION "\n");
			exit(EXIT_SUCCESS);
		} else if (strcmp("-h", arg) == 0 ||
				strcmp("--help", arg) == 0) {
			printf(usage);
			exit(EXIT_SUCCESS);
		} else if (strcmp("-o", arg) == 0 ||
				strcmp("--outpath", arg) == 0) {
			if (argc <= i + 1) {
				missing_val(arg);
			}
			outpath = path_from_opt(argv[++i]);
		} else if (strcmp("-p", arg) == 0 ||
				strcmp("--printable", arg) == 0) {
			only_printable = true;
		} else if (strcmp("-s", arg) == 0 ||
				strcmp("--oneline", arg) == 0) {
			oneline = true;
		} else if (strcmp("-r", arg) == 0 ||
				strcmp("--raw", arg) == 0) {
			escape = false;
		} else if (strcmp("-q", arg) == 0 ||
				strcmp("--quote", arg) == 0) {
			quote = true;
		} else if ((new_fmt = fmt_from_opt(arg))) {
			fmt = new_fmt;
		} else if (arg[0] == '-') {
			fprintf(stderr, PROG_NAME ": "
				"unrecognized option: %s, try -h\n", arg);
			exit(EXIT_FAILURE);
		} else if (inpath) {
			fprintf(stderr, PROG_NAME ": "
				"unexpected argument: %s, try -h\n", arg);
			exit(EXIT_FAILURE);
		} else {
			inpath = path_from_opt(arg);
		}
	}

	FILE* infile = open_or_default(inpath, "r", stdin);
	FILE* outfile = open_or_default(outpath, "w", stdout);

	int ch;
	bool first = true;
	while ((ch = fgetc(infile)) != EOF) {
		if (only_printable && !isprint(ch)) {
			continue;
		}

		if (first) {
			first = false;
		} else if (oneline) {
			fputc(' ', outfile);
		}

		if (quote) {
			fputc('"', outfile);
		}

		if (escape && fmt[1] == 'c' && (
				!isprint(ch) ||
				(ch == ' ' && oneline && !quote))) {
			fputs(escape_char(ch), outfile);
		} else {
			fprintf(outfile, fmt, ch);
		}

		if (quote) {
			fputc('"', outfile);
		}

		if (!oneline) {
			fputc('\n', outfile);
		}
	}

	if (oneline) {
		fputc('\n', outfile);
	}

	if (ferror(infile)) {
		perror(PROG_NAME);
		exit(EXIT_FAILURE);
	}

	return 0;
}
