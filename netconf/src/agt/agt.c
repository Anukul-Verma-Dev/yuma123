/*  FILE: agt.c

                
*********************************************************************
*                                                                   *
*                  C H A N G E   H I S T O R Y                      *
*                                                                   *
*********************************************************************

date         init     comment
----------------------------------------------------------------------
04nov05      abb      begun

*********************************************************************
*                                                                   *
*                     I N C L U D E    F I L E S                    *
*                                                                   *
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _H_procdefs
#include  "procdefs.h"
#endif

#ifndef _H_agt
#include "agt.h"
#endif

#ifndef _H_agt_acm
#include "agt_acm.h"
#endif

#ifndef _H_agt_cap
#include "agt_cap.h"
#endif

#ifndef _H_agt_cb
#include "agt_cb.h"
#endif

#ifndef _H_agt_cli
#include "agt_cli.h"
#endif

#ifndef _H_agt_connect
#include "agt_connect.h"
#endif

#ifndef _H_agt_hello
#include "agt_hello.h"
#endif

#ifndef _H_agt_if
#include "agt_if.h"
#endif

#ifndef _H_agt_ncx
#include "agt_ncx.h"
#endif

#ifndef _H_agt_not
#include "agt_not.h"
#endif

#ifndef _H_agt_proc
#include "agt_proc.h"
#endif

#ifndef _H_agt_rpc
#include "agt_rpc.h"
#endif

#ifndef _H_agt_ses
#include "agt_ses.h"
#endif

#ifndef _H_agt_signal
#include "agt_signal.h"
#endif

#ifndef _H_agt_state
#include "agt_state.h"
#endif

#ifndef _H_agt_sys
#include "agt_sys.h"
#endif

#ifndef _H_agt_val
#include "agt_val.h"
#endif

#ifndef _H_agt_timer
#include "agt_timer.h"
#endif

#ifndef _H_log
#include "log.h"
#endif

#ifndef _H_ncx
#include "ncx.h"
#endif

#ifndef _H_ncxconst
#include "ncxconst.h"
#endif

#ifndef _H_ncxmod
#include "ncxmod.h"
#endif

#ifndef _H_status
#include  "status.h"
#endif


/********************************************************************
*                                                                   *
*                       C O N S T A N T S                           *
*                                                                   *
*********************************************************************/

#ifdef DEBUG
#define AGT_DEBUG 1
#endif


/********************************************************************
*                                                                   *
*                       V A R I A B L E S                            *
*                                                                   *
*********************************************************************/
static boolean            agt_init_done = FALSE;
static agt_profile_t      agt_profile;
static boolean            agt_shutdown;
static boolean            agt_shutdown_started;
static ncx_shutdowntyp_t  agt_shutmode;

/********************************************************************
* FUNCTION init_agent_profile
* 
* Hardwire the initial agent profile variables
*
* OUTPUTS:
*  *agt_profile is filled in with params of defaults
*
*********************************************************************/
static void
    init_agent_profile (void)
{
    memset(&agt_profile, 0x0, sizeof(agt_profile_t));

    dlq_createSQue(&agt_profile.agt_savedevQ);

    /* Set the default values for the user parameters
     * these may be overridden from the command line;
     */
    agt_profile.agt_targ = NCX_AGT_TARG_CANDIDATE;
    agt_profile.agt_start = NCX_AGT_START_MIRROR;
    agt_profile.agt_has_startup = FALSE;
    agt_profile.agt_loglevel = log_get_debug_level();
    agt_profile.agt_usestartup = TRUE;
    agt_profile.agt_logappend = FALSE;
    agt_profile.agt_xmlorder = FALSE;
    agt_profile.agt_deleteall_ok = FALSE;
    agt_profile.agt_conffile = NULL;
    agt_profile.agt_logfile = NULL;
    agt_profile.agt_startup = NULL;
    agt_profile.agt_modpath = NULL;
    agt_profile.agt_datapath = NULL;
    agt_profile.agt_runpath = NULL;
    agt_profile.agt_defaultStyle = NCX_EL_REPORT_ALL;
    agt_profile.agt_defaultStyleEnum = NCX_WITHDEF_REPORT_ALL;
    agt_profile.agt_superuser = NCX_DEF_SUPERUSER;
    agt_profile.agt_eventlog_size = 1000;
    agt_profile.agt_maxburst = 10;
    agt_profile.agt_usevalidate = TRUE;
    agt_profile.agt_hello_timeout = 300;
    agt_profile.agt_idle_timeout = 3600;

} /* init_agent_profile */


/********************************************************************
* FUNCTION clean_agent_profile
* 
* Clean the agent profile variables
*
* OUTPUTS:
*  *agt_profile is filled in with params of defaults
*
*********************************************************************/
static void
    clean_agent_profile (void)
{
    ncx_clean_save_deviationsQ(&agt_profile.agt_savedevQ);

} /* clean_agent_profile */


/********************************************************************
* FUNCTION load_running_config
* 
* Load the NV startup config into the running config
* 
* INPUTS:
*   startup == startup filespec provided by the user
*           == NULL if not set by user 
*              (use default name and specified search path instead)
*
* OUTPUTS:
*   The <running> config is loaded from NV-storage,
*   if the NV-storage <startup> config can be found an read
*********************************************************************/
static void
    load_running_config (const xmlChar *startup)
{
    cfg_template_t  *cfg;
    xmlChar         *fname;
    status_t         res;

    cfg = cfg_get_config(NCX_CFG_RUNNING);
    if (!cfg) {
        log_error("\nagt: No running config found!!");
        SET_ERROR(ERR_INTERNAL_VAL);
        return;
    }

    /* use the user-set startup or default filename */
    if (startup) {
        /* relative filespec, use search path */
        fname = ncxmod_find_data_file(startup, FALSE, &res);
    } else {
        /* search for the default startup-cfg.xml filename */
        fname = ncxmod_find_data_file(NCX_DEF_STARTUP_FILE, FALSE, &res);
    }

    /* check if error finding the filespec */
    if (startup && !fname) {
        log_error("\nWarning: Startup config file (%s) not found."
                 "\n   Booting with empty configuration!\n",
                  startup);
    }

    if (!fname) {
        return;
    }
    
    /* try to load the config file that was found or given */
    res = agt_ncx_cfg_load(cfg, CFG_LOC_FILE, fname);
    if (res != NO_ERR) {
        if (!dlq_empty(&cfg->load_errQ)) {
            log_error("\nError: configuration errors occurred loading the "
                     "<running> database from NV-storage"
                     "\n     (%s)\n", 
                      fname);
        } else if (res == ERR_XML_READER_START_FAILED) {
            log_error("\nagt: Error: Could not open startup config file"
                      "\n     (%s)\n", 
                      fname);
        }
    }

    if (res == NO_ERR) {
        log_info("\nagt: Startup config loaded OK\n     Source: %s\n",
                 fname);
    }

    if (LOGDEBUG) {
        log_debug("\nContents of %s configuration:", cfg->name);
        val_dump_value(cfg->root, 0);
        log_debug("\n");
    }

    if (fname) {
        m__free(fname);
    }

} /* load_running_config */


/**************    E X T E R N A L   F U N C T I O N S **********/


/********************************************************************
* FUNCTION agt_init1
* 
* Initialize the Agent Library: stage 1: CLI and profile
* 
* TBD -- put platform-specific agent init here
*
* INPUTS:
*   argc == command line argument count
*   argv == array of command line strings
*   showver == address of version return quick-exit status
*   showhelp == address of help return quick-exit status
*
* OUTPUTS:
*   *showver == TRUE if user requsted version quick-exit mode
*   *showhelp == TRUE if user requsted help quick-exit mode
(
* RETURNS:
*   status of the initialization procedure
*********************************************************************/
status_t 
    agt_init1 (int argc,
               const char *argv[],
               boolean *showver,
               help_mode_t *showhelpmode)
{
    status_t  res;

    if (agt_init_done) {
        return NO_ERR;
    }

#ifdef AGT_DEBUG
    log_debug3("\nAgent Init Starting...");
#endif

    res = NO_ERR;

    /* initialize shutdown variables */
    agt_shutdown = FALSE;
    agt_shutdown_started = FALSE;
    agt_shutmode = NCX_SHUT_NONE;

    *showver = FALSE;
    *showhelpmode = HELP_MODE_NONE;

    /* allow cleanup to run even if this fn does not complete */
    agt_init_done = TRUE;

    init_agent_profile();

    /* get the command line params and also any config file params */
    res = agt_cli_process_input(argc, 
                                argv, 
                                &agt_profile,
                                showver, 
                                showhelpmode);
    if (res != NO_ERR) {
        return res;
    } /* else the agt_profile is filled in */

    /* check if quick exit mode */
    if (*showver || (*showhelpmode != HELP_MODE_NONE)) {
        return NO_ERR;
    }

    /* loglevel and log file already set */

    /* set the module search path */
    if (agt_profile.agt_modpath) {
        ncxmod_set_modpath(agt_profile.agt_modpath);
    }

    /* set the data file search path */
    if (agt_profile.agt_datapath) {
        ncxmod_set_datapath(agt_profile.agt_datapath);
    }

    /* set the script file search path (NOT USED YET!) */
    if (agt_profile.agt_runpath) {
        ncxmod_set_runpath(agt_profile.agt_runpath);
    }

    return res;

} /* agt_init1 */


/********************************************************************
* FUNCTION agt_init2
* 
* Initialize the Agent Library
* 
* TBD -- put platform-specific agent init here
*
* RETURNS:
*   status
*********************************************************************/
status_t 
    agt_init2 (void)
{
    const val_value_t  *clivalset;
    val_value_t        *val;
    status_t            res;

#ifdef AGT_DEBUG
    log_debug3("\nAgent Init-2 Starting...");
#endif

    /* init user callback support */
    agt_cb_init();

    /* initial signal handler first to allow clean exit */
    agt_signal_init();

    /* initialize the agent timer service */
    agt_timer_init();
    
    /* initialize the RPC agent callback structures */
    res = agt_rpc_init();
    if (res != NO_ERR) {
        return res;
    }

    /* initialize the NCX connect handler */
    res = agt_connect_init();
    if (res != NO_ERR) {
        return res;
    }

    /* initialize the NCX hello handler */
    res = agt_hello_init();
    if (res != NO_ERR) {
        return res;
    }


    /* setup an empty <running> config 
     * The config state is still CFG_ST_INIT
     * so no user access can occur yet (except OP_LOAD by root)
     */
    res = cfg_init_static_db(NCX_CFGID_RUNNING);
    if (res != NO_ERR) {
        return res;
    }

    /*** All Agent profile parameters should be set by now ***/

    /* must set the agent capabilities after the profile is set */
    res = agt_cap_set_caps(agt_profile.agt_targ, 
                           agt_profile.agt_start,
                           agt_profile.agt_defaultStyle);
    if (res != NO_ERR) {
        return res;
    }

    /* setup the candidate config if it is used */
    if (agt_profile.agt_targ==NCX_AGT_TARG_CANDIDATE) {
        res = cfg_init_static_db(NCX_CFGID_CANDIDATE);
        if (res != NO_ERR) {
            return res;
        }
    }

    /* setup the startup config if it is used */
    if (agt_profile.agt_start==NCX_AGT_START_DISTINCT) {
        res = cfg_init_static_db(NCX_CFGID_STARTUP);
        if (res != NO_ERR) {
            return res;
        }
    }

    /* initialize the NCX agent core callback functions
     * the schema (netconf.ncx) for these callbacks was 
     * already loaded in the common ncx_init
     * */
    res = agt_ncx_init();
    if (res != NO_ERR) {
        return res;
    }

    /* initialize the agent access control model */
    res = agt_acm_init();
    if (res != NO_ERR) {
        return res;
    }

    /* initialize the session handler data structures */
    agt_ses_init();

    /* load the system module */
    res = agt_sys_init();
    if (res != NO_ERR) {
        return res;
    }

    /* load the NETCONF state monitoring data model module */
    res = agt_state_init();
    if (res != NO_ERR) {
        return res;
    }

    /* load the NETCONF Notifications data model module */
    res = agt_not_init();
    if (res != NO_ERR) {
        return res;
    }

    /* load the NETCONF /proc monitoring data model module */
    res = agt_proc_init();
    if (res != NO_ERR) {
        return res;
    }

    /* load the NETCONF interface monitoring data model module */
    res = agt_if_init();
    if (res != NO_ERR) {
        return res;
    }
    
    /* check the module parameter set from CLI or conf file
     * for any modules to pre-load
     */
    res = NO_ERR;
    clivalset = agt_cli_get_valset();
    if (clivalset) {

        /* first check if there are any deviations to load */
        val = val_find_child(clivalset, 
                             NCXMOD_NETCONFD, 
                             NCX_EL_DEVIATION);
        while (val) {
            res = ncxmod_load_deviation(VAL_STR(val), 
                                        &agt_profile.agt_savedevQ);
            if (res != NO_ERR) {
                return res;
            } else {
                val = val_find_next_child(clivalset,
                                          NCXMOD_NETCONFD,
                                          NCX_EL_DEVIATION,
                                          val);
            }
        }

        val = val_find_child(clivalset,
                             NCXMOD_NETCONFD,
                             NCX_EL_MODULE);

        while (val && res == NO_ERR) {
            res = ncxmod_load_module(VAL_STR(val),
                                     NULL,   /* parse revision TBD */
                                     &agt_profile.agt_savedevQ,
                                     NULL);
            if (res == NO_ERR) {
                /*** TBD: load lib<modname>.<rev-date>.so 
                 *** into the agent code
                 ***/

                val = val_find_next_child(clivalset,
                                          NCXMOD_NETCONFD,
                                          NCX_EL_MODULE,
                                          val);
            }
        }
    }

    /*** ALL INITIAL YANG MODULES SHOULD BE LOADED AT THIS POINT ***/
    if (res != NO_ERR) {
        log_error("\nError: one or more modules could not be loaded");
        return ERR_NCX_OPERATION_FAILED;
    }

    if (ncx_any_mod_errors()) {
        log_error("\nError: one or more pre-loaded YANG modules"
                  " has fatal errors\n");
        return ERR_NCX_OPERATION_FAILED;
    }

    /* set the initial module capabilities in the agent <hello> message */
    res = agt_cap_set_modules();
    if (res != NO_ERR) {
        return res;
    }

    /* load the NV startup config into the running config if it exists */
    if (agt_profile.agt_usestartup) {
        load_running_config(agt_profile.agt_startup);
    } else {
        log_info("\nagt: Startup configuration skipped due "
                 "to no-startup CLI option\n");
    }

    /****   P H A S E   2   I N I T  ****  C O N F I G   D A T A  ***/

    /* load the nacm access control DM module */
    res = agt_acm_init2();
    if (res != NO_ERR) {
        return res;
    }

    /* load the system module callback functions and data */
    res = agt_sys_init2();
    if (res != NO_ERR) {
        return res;
    }
    
    /* load the agent sessions callback functions and data */
    agt_ses_init2();

    /* load the agent state monitoring callback functions and data */
    res = agt_state_init2();
    if (res != NO_ERR) {
        return res;
    }

    /* load the notifications callback functions and data */
    res = agt_not_init2();
    if (res != NO_ERR) {
        return res;
    }

    /* load the /proc monitoring callback functions and data */
    res = agt_proc_init2();
    if (res != NO_ERR) {
        return res;
    }

    /* load the interface monitoring callback functions and data */
    res = agt_if_init2();
    if (res != NO_ERR) {
        return res;
    }

    /* allow users to access the configuration databases now */
    cfg_set_state(NCX_CFGID_RUNNING, CFG_ST_READY);

    /* set the correct configuration target */
    if (agt_profile.agt_targ==NCX_AGT_TARG_CANDIDATE) {
        res = cfg_fill_candidate_from_running();
        if (res != NO_ERR) {
            return res;
        }
        cfg_set_state(NCX_CFGID_CANDIDATE, CFG_ST_READY);
        cfg_set_target(NCX_CFGID_CANDIDATE);
    } else {
        cfg_set_target(NCX_CFGID_RUNNING);
    }

    /* setup the startup config only if used */
    if (agt_profile.agt_start==NCX_AGT_START_DISTINCT) {
        cfg_set_state(NCX_CFGID_STARTUP, CFG_ST_READY);
    }

    /* data modules can be accessed now, and still added
     * and deleted dynamically as well
     *
     * No sessions can actually be started until the 
     * agt_ncxserver_run function is called from netconfd_run
     */

    return NO_ERR;

}  /* agt_init2 */


/********************************************************************
* FUNCTION agt_cleanup
*
* Cleanup the Agent Library
* 
* TBD -- put platform-specific agent cleanup here
*
*********************************************************************/
void
    agt_cleanup (void)
{
    if (agt_init_done) {
#ifdef AGT_DEBUG
        log_debug3("\nAgent Cleanup Starting...\n");
#endif

        clean_agent_profile();
        agt_acm_cleanup();
        agt_ncx_cleanup();
        agt_hello_cleanup();
        agt_cli_cleanup();
        agt_sys_cleanup();
        agt_state_cleanup();
        agt_not_cleanup();
        agt_proc_cleanup();
        agt_if_cleanup();
        agt_ses_cleanup();
        agt_cap_cleanup();
        agt_rpc_cleanup();
        agt_signal_cleanup();
        agt_timer_cleanup();
        agt_connect_cleanup();
        agt_cb_cleanup();

        print_errors();

        log_close();

        agt_init_done = FALSE;
    }
}   /* agt_cleanup */


/********************************************************************
* FUNCTION agt_get_profile
* 
* Get the agent profile struct
* 
* INPUTS:
*   none
* RETURNS:
*   const pointer to the agent profile
*********************************************************************/
agt_profile_t *
    agt_get_profile (void)
{
    return &agt_profile;

} /* agt_get_profile */


/********************************************************************
* FUNCTION agt_request_shutdown
* 
* Request some sort of agent shutdown
* 
* INPUTS:
*   mode == requested shutdown mode
*
*********************************************************************/
void
    agt_request_shutdown (ncx_shutdowntyp_t mode)
{
#ifdef DEBUG
    if (mode <= NCX_SHUT_NONE || mode > NCX_SHUT_EXIT) {
        SET_ERROR(ERR_INTERNAL_VAL);
        return;
    }
#endif

    /* don't allow the shutdown mode to change in mid-stream */
    if (agt_shutdown_started) {
        return;
    }

    agt_shutdown = TRUE;
    agt_shutdown_started = TRUE;
    agt_shutmode = mode;

}  /* agt_request_shutdown */


/********************************************************************
* FUNCTION agt_shutdown_requested
* 
* Check if some sort of agent shutdown is in progress
* 
* RETURNS:
*    TRUE if shutdown mode has been started
*
*********************************************************************/
boolean
    agt_shutdown_requested (void)
{
    return agt_shutdown;

}  /* agt_shutdown_requested */


/********************************************************************
* FUNCTION agt_shutdown_mode_requested
* 
* Check what shutdown mode was requested
* 
* RETURNS:
*    shutdown mode
*
*********************************************************************/
ncx_shutdowntyp_t
    agt_shutdown_mode_requested (void)
{
    return agt_shutmode;

}  /* agt_shutdown_mode_requested */


/********************************************************************
* FUNCTION agt_cbtype_name
* 
* Get the string for the agent callback phase
* 
* INPUTS:
*   cbtyp == callback type enum
*
* RETURNS:
*    const string for this enum
*********************************************************************/
const xmlChar *
    agt_cbtype_name (agt_cbtyp_t cbtyp)
{
    switch (cbtyp) {
    case AGT_CB_LOAD_MOD:
        return (const xmlChar *)"load-module";
    case AGT_CB_UNLOAD_MOD:
        return (const xmlChar *)"unload-module";
    case AGT_CB_VALIDATE:
        return (const xmlChar *)"validate";
    case AGT_CB_APPLY:
        return (const xmlChar *)"apply";
    case AGT_CB_COMMIT:
        return (const xmlChar *)"commit";
    case AGT_CB_ROLLBACK:
        return (const xmlChar *)"rollback";
    case AGT_CB_TEST_APPLY:
        return (const xmlChar *)"test-apply";
    default:
        SET_ERROR(ERR_INTERNAL_VAL);
        return (const xmlChar *)"invalid";
    }
}  /* agt_cbtyp_name */


/* END file agt.c */
