#ifndef CC_PARSE_H
#define CC_PARSE_H 1

static void
parse_argv(const int argc,const char **argv ) 
{
  for ( ;; ) {
    opterr = 0;
    int c = getopt_long( *argc, *argv, short_options, long_options, NULL );

    if ( c == -1 ) {
      break;
    }

    switch ( c ) {
   /*   case 'n':
        set_trema_name( optarg );
        break;
      case 'd':
        run_as_daemon = true;
        break;
      case 'l':
        set_logging_level( optarg );
        break;
      case 'g':
        log_output_type = LOGGING_TYPE_SYSLOG;
        break;
      case 'f':
        set_syslog_facility( optarg );
        break;
      case 'h':
        usage();
        xfree( trema_name );
        xfree( executable_name );
        exit( EXIT_SUCCESS );
        break;
      default:
        continue;
    }

    if ( optarg == NULL || strchr( new_argv[ optind - 1 ], '=' ) != NULL ) {
      argc_tmp -= 1;
      new_argv[ optind - 1 ] = NULL;
    }
    else {
      argc_tmp -= 2;
      new_argv[ optind - 1 ] = NULL;
      new_argv[ optind - 2 ] = NULL;
    }
  }

  for ( int i = 0, j = 0; i < *argc; ++i ) {
    if ( new_argv[ i ] != NULL ) {
      ( *argv )[ j ] = new_argv[ i ];
      j++;
    }
  }
  if ( argc_tmp < *argc ) {
    ( *argv )[ argc_tmp ] = NULL;
  }
  *argc = argc_tmp;

  reset_getopt();*/
}

#endif