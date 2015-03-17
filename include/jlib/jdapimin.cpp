//-------------------------------------------------------------------------//
//          Windows Graphics Programming: Win32 GDI and DirectDraw         //
//                        ISBN  0-13-086985-6                              //
//                                                                         //
//  Modified by: Yuan, Feng                             www.fengyuan.com   //
//  Changes    : C++, exception, in-memory source, BGR byte order          //
//  Version    : 1.00.000, May 31, 2000                                    //
//-------------------------------------------------------------------------//

/*
 * jdapimin.c
 *
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains application interface code for the decompression half
 * of the JPEG library.  These are the "minimum" API routines that may be
 * needed in either the normal full-decompression case or the
 * transcoding-only case.
 *
 * Most of the routines intended to be called directly by an application
 * are in this file or in jdapistd.c.  But also see jcomapi.c for routines
 * shared by compression and decompression, and jdtrans.c for the transcoding
 * case.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"


/*
 * Initialization of a JPEG decompression object.
 * The error manager must already be set up (in case memory manager fails).
 */

void jpeg_decompress_struct::jpeg_CreateDecompress(int version, size_t structsize)
{
	int i;

	/* Guard against version mismatches between library and caller. */
	mem = NULL;		/* so jpeg_destroy knows mem mgr not called */
  
	if (version != JPEG_LIB_VERSION)
		ERREXIT2(JERR_BAD_LIB_VERSION, JPEG_LIB_VERSION, version);
  
	if (structsize != sizeof(jpeg_decompress_struct))
		ERREXIT2(JERR_BAD_STRUCT_SIZE, 
	     (int) sizeof(jpeg_decompress_struct), (int) structsize);

	/* For debugging purposes, we zero the whole master structure.
	* But the application has already set the err pointer, and may have set
	* client_data, so we have to save and restore those fields.
	* Note: if application hasn't set client_data, tools like Purify may
	* complain here.
	*/
	/*
	{
		struct jpeg_error_mgr * e = err;
		void * c_data = client_data; // ignore Purify complaint here 
		memset(this, 0, sizeof(jpeg_decompress_struct));
		
		err = e
		client_data = c_data;
	}
	*/
	is_decompressor = TRUE;

	/* Initialize a memory manager instance for this object */
	jinit_memory_mgr(this);

	/* Zero out pointers to permanent structures. */
	progress = NULL;
	src = NULL;

	for (i = 0; i < NUM_QUANT_TBLS; i++)
		quant_tbl_ptrs[i] = NULL;

	for (i = 0; i < NUM_HUFF_TBLS; i++) 
	{
		dc_huff_tbl_ptrs[i] = NULL;
		ac_huff_tbl_ptrs[i] = NULL;
	}

	/* Initialize marker processor so application can override methods
	* for COM, APPn markers before calling jpeg_read_header.
	*/
	marker_list = NULL;
	jinit_marker_reader(this);

	/* And initialize the overall input controller. */
	jinit_input_controller(this);

	/* OK, I'm ready */
	global_state = DSTATE_START;
}


// Destruction of a JPEG decompression object
void jpeg_decompress_struct::jpeg_destroy_decompress(void)
{
	jpeg_destroy(); /* use common routine */
}


// Abort processing of a JPEG decompression operation,
// but don't destroy the object itself.
void jpeg_decompress_struct::jpeg_abort_decompress(void)
{
	jpeg_abort((j_common_ptr) this); /* use common routine */
}


// Set default decompression parameters.
void jpeg_decompress_struct::default_decompress_parms(void)
{
	/* Guess the input colorspace, and set output colorspace accordingly. */
	/* (Wish JPEG committee had provided a real way to specify this...) */
	/* Note application may override our guesses. */
	switch (num_components) 
	{
		case 1:
			jpeg_color_space = JCS_GRAYSCALE;
			out_color_space = JCS_GRAYSCALE;
			break;
    
		case 3:
			if (saw_JFIF_marker) 
			{
				jpeg_color_space = JCS_YCbCr; /* JFIF implies YCbCr */
			} 
			else if (saw_Adobe_marker) 
			{
				switch (Adobe_transform) 
				{
					case 0: jpeg_color_space = JCS_RGB;
							break;
					case 1: jpeg_color_space = JCS_YCbCr;
							break;
					default:
							WARNMS1(JWRN_ADOBE_XFORM, Adobe_transform);
							jpeg_color_space = JCS_YCbCr; /* assume it's YCbCr */
							break;
				}
			} 
			else 
			{
				/* Saw no special markers, try to guess from the component IDs */
				int cid0 = comp_info[0].component_id;
				int cid1 = comp_info[1].component_id;
				int cid2 = comp_info[2].component_id;

				if (cid0 == 1 && cid1 == 2 && cid2 == 3)
					jpeg_color_space = JCS_YCbCr; /* assume JFIF w/out marker */
				else if (cid0 == 82 && cid1 == 71 && cid2 == 66)
					jpeg_color_space = JCS_RGB; /* ASCII 'R', 'G', 'B' */
				else 
				{
					TRACEMS3(this, 1, JTRC_UNKNOWN_IDS, cid0, cid1, cid2);
					jpeg_color_space = JCS_YCbCr; /* assume it's YCbCr */
				}
			}
			/* Always guess RGB is proper output colorspace. */
			out_color_space = JCS_RGB;
			break;
    
		case 4:
			if (saw_Adobe_marker) 
			{
				switch (Adobe_transform) 
				{
					case 0: jpeg_color_space = JCS_CMYK;
							break;
					case 2: jpeg_color_space = JCS_YCCK;
							break;
					default:WARNMS1(JWRN_ADOBE_XFORM, Adobe_transform);
							jpeg_color_space = JCS_YCCK; /* assume it's YCCK */
							break;
				}
			} 
			else 
			{
				/* No special markers, assume straight CMYK. */
				jpeg_color_space = JCS_CMYK;
			}
			out_color_space = JCS_CMYK;
			break;
    
		default:
			jpeg_color_space = JCS_UNKNOWN;
			out_color_space = JCS_UNKNOWN;
			break;
	}

	/* Set defaults for other decompression parameters. */
	scale_num = 1;		/* 1:1 scaling */
	scale_denom = 1;
	output_gamma = 1.0;
	buffered_image = FALSE;
	raw_data_out = FALSE;
	dct_method = JDCT_DEFAULT;
	do_fancy_upsampling = TRUE;
	do_block_smoothing = TRUE;
	quantize_colors = FALSE;
	/* We set these in case application only sets quantize_colors. */
	dither_mode = JDITHER_FS;
#ifdef QUANT_2PASS_SUPPORTED
	two_pass_quantize = TRUE;
#else
	two_pass_quantize = FALSE;
#endif
	desired_number_of_colors = 256;
	colormap = NULL;
	/* Initialize for no mode change in buffered-image mode. */
	enable_1pass_quant = FALSE;
	enable_external_quant = FALSE;
	enable_2pass_quant = FALSE;
}


/*
 * Decompression startup: read start of JPEG datastream to see what's there.
 * Need only initialize JPEG object and supply a data source before calling.
 *
 * This routine will read as far as the first SOS marker (ie, actual start of
 * compressed data), and will save all tables and parameters in the JPEG
 * object.  It will also initialize the decompression parameters to default
 * values, and finally return JPEG_HEADER_OK.  On return, the application may
 * adjust the decompression parameters and then call jpeg_start_decompress.
 * (Or, if the application only wanted to determine the image parameters,
 * the data need not be decompressed.  In that case, call jpeg_abort or
 * jpeg_destroy to release any temporary space.)
 * If an abbreviated (tables only) datastream is presented, the routine will
 * return JPEG_HEADER_TABLES_ONLY upon reaching EOI.  The application may then
 * re-use the JPEG object to read the abbreviated image datastream(s).
 * It is unnecessary (but OK) to call jpeg_abort in this case.
 * The JPEG_SUSPENDED return code only occurs if the data source module
 * requests suspension of the decompressor.  In this case the application
 * should load more source data and then re-call jpeg_read_header to resume
 * processing.
 * If a non-suspending data source is used and require_image is TRUE, then the
 * return code need not be inspected since only JPEG_HEADER_OK is possible.
 *
 * This routine is now just a front end to jpeg_consume_input, with some
 * extra error checking.
 */

int jpeg_decompress_struct::jpeg_read_header (boolean require_image)
{
  int retcode;

  if (global_state != DSTATE_START &&
      global_state != DSTATE_INHEADER)
    ERREXIT1(JERR_BAD_STATE, global_state);

  retcode = jpeg_consume_input();

  switch (retcode) {
  case JPEG_REACHED_SOS:
    retcode = JPEG_HEADER_OK;
    break;
  case JPEG_REACHED_EOI:
    if (require_image)		/* Complain if application wanted an image */
		ERREXIT(JERR_NO_IMAGE);
    /* Reset to start state; it would be safer to require the application to
     * call jpeg_abort, but we can't change it now for compatibility reasons.
     * A side effect is to free any temporary memory (there shouldn't be any).
     */
    jpeg_abort((j_common_ptr) this); /* sets state = DSTATE_START */
    retcode = JPEG_HEADER_TABLES_ONLY;
    break;
  case JPEG_SUSPENDED:
    /* no work */
    break;
  }

  return retcode;
}


/*
 * Consume data in advance of what the decompressor requires.
 * This can be called at any time once the decompressor object has
 * been created and a data source has been set up.
 *
 * This routine is essentially a state machine that handles a couple
 * of critical state-transition actions, namely initial setup and
 * transition from header scanning to ready-for-start_decompress.
 * All the actual input is done via the input controller's consume_input
 * method.
 */

int jpeg_decompress_struct::jpeg_consume_input(void)
{
  int retcode = JPEG_SUSPENDED;

  /* NB: every possible DSTATE value should be listed in this switch */
  switch (global_state) {
  case DSTATE_START:
    /* Start-of-datastream actions: reset appropriate modules */
    (*inputctl->reset_input_controller) (this);
    /* Initialize application's data source module */
    src->init_source(this);
    global_state = DSTATE_INHEADER;
    /*FALLTHROUGH*/
  case DSTATE_INHEADER:
    retcode = (*inputctl->consume_input) (this);
    if (retcode == JPEG_REACHED_SOS) { /* Found SOS, prepare to decompress */
      /* Set up default parameters based on header data */
      default_decompress_parms();
      /* Set global state: ready for start_decompress */
      global_state = DSTATE_READY;
    }
    break;
  case DSTATE_READY:
    /* Can't advance past first SOS until start_decompress is called */
    retcode = JPEG_REACHED_SOS;
    break;
  case DSTATE_PRELOAD:
  case DSTATE_PRESCAN:
  case DSTATE_SCANNING:
  case DSTATE_RAW_OK:
  case DSTATE_BUFIMAGE:
  case DSTATE_BUFPOST:
  case DSTATE_STOPPING:
    retcode = (*inputctl->consume_input) (this);
    break;
  default:
    ERREXIT1(JERR_BAD_STATE, global_state);
  }
  return retcode;
}


// Have we finished reading the input file?
boolean jpeg_decompress_struct::jpeg_input_complete(void)
{
	/* Check for valid jpeg object */
	if ( global_state < DSTATE_START || global_state > DSTATE_STOPPING )
		ERREXIT1(JERR_BAD_STATE, global_state);
  
	return inputctl->eoi_reached;
}


// Is there more than one scan?
boolean jpeg_decompress_struct::jpeg_has_multiple_scans (void)
{
	/* Only valid after jpeg_read_header completes */
	if ( global_state < DSTATE_READY || global_state > DSTATE_STOPPING	)
		ERREXIT1(JERR_BAD_STATE, global_state);
  
	return inputctl->has_multiple_scans;
}


/*
 * Finish JPEG decompression.
 *
 * This will normally just verify the file trailer and release temp storage.
 *
 * Returns FALSE if suspended.  The return value need be inspected only if
 * a suspending data source is used.
 */

boolean jpeg_decompress_struct::jpeg_finish_decompress(void)
{
	if ((global_state == DSTATE_SCANNING ||
       global_state == DSTATE_RAW_OK) && ! buffered_image) 
	{
		/* Terminate final pass of non-buffered mode */
		if (output_scanline < output_height)
			ERREXIT(JERR_TOO_LITTLE_DATA);
    
		(*master->finish_output_pass) (this);
		global_state = DSTATE_STOPPING;
	} 
	else if (global_state == DSTATE_BUFIMAGE) 
	{
		/* Finishing after a buffered-image operation */
		global_state = DSTATE_STOPPING;
	} 
	else if (global_state != DSTATE_STOPPING) 
	{
		/* STOPPING = repeat call after a suspension, anything else is error */
		ERREXIT1(JERR_BAD_STATE, global_state);
	}
	
	/* Read until EOI */
	while (! inputctl->eoi_reached) 
	{
		if ((*inputctl->consume_input) (this) == JPEG_SUSPENDED)
			return FALSE;		/* Suspend, come back later */
	}
  
	/* Do final cleanup */
	src->term_source(this);
  
	/* We can use jpeg_abort to release memory and reset global_state */
	jpeg_abort(this);
	return TRUE;
}
