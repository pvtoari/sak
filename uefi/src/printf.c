#include <stdarg.h>

#include "efi/efi_common.h"
#include "efi/efi_simple_io.h"

// base 16 max
bool print_number(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *f, UINTN number, UINT8 base, UINT8 lengthSpecifier, bool is_signed) {
  CHAR16 *digits = u"0123456789ABCDEF";
  CHAR16 buffer[66]; // '0b'+64bits
  bool negative = false;
  UINTN i = 0;

  if(base > 16) { /* invalid base */
    // TODO: Print error here?
    return false;
  }

  if(is_signed) {
    if((INTN)number < 0) {
      number = -(INTN)number; // get abs value to get digits to print
      negative = true;
    }
  }

  do {
    buffer[i++] = digits[number % base];
    number /= base;
  } while(number > 0);

  switch(base) {
    case 2: /* Binary */
      buffer[i++] = u'b';
      buffer[i++] = u'0';
      break;
    case 8: /* Octal */
      buffer[i++] = u'o';
      buffer[i++] = u'0';
      break;
    case 10: /* decimal */ break;
    case 16: /* hexadecimal */
      buffer[i++] = u'x';
      buffer[i++] = u'0';
      break;
    default: /* Invalid ? */
      break;
  }

  while(i < lengthSpecifier) buffer[i++] = u'0'; // pad with 0s
  // NOTE: This will only happen if is_signed
  if(negative) buffer[i++] = u'-';
  buffer[i--] = u'\0'; // null terminate

  // reverse digits in buffer
  for(UINTN j = 0; j < i; j++, i--) {
    CHAR16 temp = buffer[j];
    buffer[j] = buffer[i];
    buffer[i] = temp;
  }

  f->OutputString(f, buffer);

  return true;
}

// Print formatted strings
bool fprintf(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *f, CHAR16 *fmt, ...) {
  bool result = true;
  CHAR16 charstr[2];  // TODO: Replace initializing this with memset and use = { } initializer
  va_list args;

  va_start(args, fmt);

  // Initialize buffers
  charstr[0] = u'\0', charstr[1] = u'\0';

  // Print formatted string values
  for (UINTN i = 0; fmt[i] != u'\0'; i++) {
    if (fmt[i] == u'%') {
      i++;

      int lengthModifier = 32; // 32 bit default
      UINT8 lengthSpecifier = 0; // minimum output length
      UINT8 base = 10;
      bool isnumber = false;
      bool issigned = false;
      if(fmt[i] != u'\0' && fmt[i+1] != u'\0') {
        if(fmt[i] == u'l') {
          lengthModifier = 64;
          i++;
        }
        else if(fmt[i] >= u'0' && fmt[i] <= u'9') {
          lengthSpecifier = (UINT8)(fmt[i] - u'0');
          i++;
        }
      }

      // Grab next argument type from input args, and print it
      switch (fmt[i]) {
        case u'b': {
          base = 2;
          isnumber = true;
        } break;

        case u'c': {
          charstr[0] = (CHAR16)va_arg(args, int); // NOTE: A va_arg must be at least 32 bits
          f->OutputString(f, charstr);
        } break;

        case u'd': {
          isnumber = true;
          issigned = true;
        } break;

        case u'o': {
          isnumber = true;
          base = 8;
        } break;

        case u's': {
          // Print CHAR16 string; printf("%s", string)
          CHAR16 *string = va_arg(args, CHAR16*);
          f->OutputString(f, string);
        } break;

        case u'u': {
          isnumber = true;
        } break;

        case u'x': {
          isnumber = true;
          base = 16;
        } break;

        default: {
          f->OutputString(f, u"Invalid format specifier: '%");
          charstr[0] = fmt[i];
          f->OutputString(f, charstr);
          f->OutputString(f, u"'\r\n");
          result = false;
          goto endprintf;
        } break;
      }

      if(isnumber) {
        if(lengthModifier == 64) {
          if(issigned) {
            INT64 number = va_arg(args, INT64);
            print_number(f, number, base, lengthSpecifier, issigned);
          } else {
            UINT64 number = va_arg(args, UINT64);
            print_number(f, number, base, lengthSpecifier, issigned);
          }
        } else {
          if(issigned) {
            INT32 number = va_arg(args, INT32);
            print_number(f, number, base, lengthSpecifier, issigned);
          } else {
            UINT32 number = va_arg(args, UINT32);
            print_number(f, number, base, lengthSpecifier, issigned);
          }
        }
      }
    } else {
      // Not formatted string, print next character
      charstr[0] = fmt[i];
      f->OutputString(f, charstr);
    }
  }

endprintf:
  va_end(args);

  return result;
}

#define printf(...) fprintf(cout, __VA_ARGS__)

bool printfln(CHAR16 *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  bool ok = printf(fmt, args);
  if(ok) cout->OutputString(cout, u"\r\n");
  va_end(args);
  return ok;
}
