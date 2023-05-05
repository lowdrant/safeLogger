/**
 * Log data to an SD card without risking file overwrites. Assumes files
 *
 * Public Methods:
 *  getLogName (File *dir, char * basename, char * sfx) -> String
 *
 * Gotchas:
 * - forces strings to lowercase for simplicity
 * - SD library forces creted file to have uppercase name (as of 2023-05-04)
 *
 */
#ifndef _SAFELOGGER_H
#define _SAFELOGGER_H

#include <SD.h>
#include <string.h>
#include <ctype.h>

/**
 * Assuming a filename of the form
 *      <basename><digit sequence>.<suffix>,
 * get <digit sequence> as a long integer.
 *
 * GOTCHA: treats first "." in fn as the start of the suffix.
 *
 * @param fn filename to get the number
 * @return num digit sequence at end of fn
 */
static long getFileNum(const char *const fn) {
    /* copy string to avoid strtok overwrite */
    char fntmp[strlen(fn) + 1];
    strncpy(fntmp, fn, strlen(fn) + 1);

    /* find digit substring */
    char *basename = strtok(fn, ".");
    size_t len = strlen(basename);
    int i;
    for (i = len - 1; i > -1; i--) {
        if (!isdigit(basename[i])) {
            break;
        }
    }

    /* substring to number */
    return atol(basename + i + 1);
}

/**
 * Assuming a sequence of files with names of the form,
  *      <basename><digit sequence>.<suffix>,
 * get the <digit sequence> as a long integer.
 *
 * WARN!!: strings must be lowercase
 *
 * @param dir SD card directory object
 * @param base <basename> of log files
 * @param sfx <suffix> of log files
 *
 * @example Say dir has the files:
 *      log0.csv, log1.csv, log4.csv.
 * Calling `getLogNum(dir, "log", "csv")` returns `5`. Note that
 * the files need not be sequential. This function simply returns
 * largest number + 1.
 *
 */
static long getLogNum(const File * const dir, const char *const base,
                      const char *const sfx) {
    /* string comparison vars */
    size_t baselen = strlen(base);
    long num = 0;
    long curnum = 0;

    /* traverse files */
    long i;
    while (true) {
        /* null check */
        File entry = dir->openNextFile();
        if (!entry) {
            break;
        }

        if (!entry.isDirectory()) {
            /* entry uppercases everything, so lower */
            char fn[strlen(entry.name()) + 1];
            for (i = 0; i < strlen(entry.name()); i++) {
                fn[i] = tolower(entry.name()[i]);
            }
            fn[i] = '\0';

            /* Check if filetype and basename match */
            if ((strstr(fn, sfx) != NULL)
                && (strncmp(base, fn, baselen) == 0)) {
                curnum = getFileNum(fn);
                if (curnum >= num) {
                    num = curnum + 1;
                }
            }
        }
        entry.close();
    }
    return num;
}

/**
 * Assuming a sequence of files of the form <basname><digits>.<suffix>, create
 * the next filename in the sequence.

 * The digits of the returned filename are one more than the largest digits,
 * e.g. for
 *      "log0.csv", "log1.csv", "log4.csv",
 * this function will return "log5.csv".
 *
 * @param dir SD card file directory object
 * @param base filename base, e.g. "log"
 * @param sfx filename suffix, e.g. "sfx"
 */
String getLogName(const File * const dir, const char *const base,
                  const char *const sfx) {
    /* enforce lower case */
    int i;
    size_t blen = strlen(base);
    char baselower[blen + 1];
    for (i = 0; i < blen; i++) baselower[i] = tolower(base[i]);
    baselower[blen] = '\0';

    size_t slen = strlen(sfx);
    char sfxlower[slen + 1];
    for (i = 0; i < slen; i++) sfxlower[i] = tolower(sfx[i]);
    sfxlower[slen] = '\0';

    /* get filename */
    int fnnum = getLogNum(dir, baselower, sfxlower);
    String fn = String(base) + String(fnnum) + "." + String(sfx);
    return fn;
}

// TODO: logname zero-padding

#endif /* ifndef _SAFELOGGER_H */
