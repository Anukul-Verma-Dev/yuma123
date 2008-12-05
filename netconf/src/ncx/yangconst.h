#ifndef _H_yangconst
#define _H_yangconst

/*  FILE: yangconst.h
*********************************************************************
*								    *
*			 P U R P O S E				    *
*								    *
*********************************************************************

  Contains YANG constants separated to prevent H file include loops
 
*********************************************************************
*								    *
*		   C H A N G E	 H I S T O R Y			    *
*								    *
*********************************************************************

date	     init     comment
----------------------------------------------------------------------
27-oct-07    abb      Begun; started from ncxconst.h
*/

#include <math.h>
#include <xmlstring.h>

#ifndef _H_xmlns
#include "xmlns.h"
#endif

/********************************************************************
*								    *
*			 C O N S T A N T S			    *
*								    *
*********************************************************************/

#define YANG_URN (const xmlChar *)"urn:ietf:params:xml:ns:yang:1"
#define YANG_PREFIX (const xmlChar *)"yang"
#define YANG_MODULE (const xmlChar *)"yang"   /***  no mod ! ***/

#define YANG_VERSION_STR          (const xmlChar *)"1"
#define YANG_VERSION_NUM          1

/* YANG keyword/YIN element names */

#define YANG_K_AFTER             (const xmlChar *)"after"
#define YANG_K_ARGUMENT          (const xmlChar *)"argument"
#define YANG_K_AUGMENT           (const xmlChar *)"augment"
#define YANG_K_ANYXML            (const xmlChar *)"anyxml"
#define YANG_K_BASE              (const xmlChar *)"base"
#define YANG_K_BEFORE            (const xmlChar *)"before"
#define YANG_K_BELONGS_TO        (const xmlChar *)"belongs-to"
#define YANG_K_BIT               (const xmlChar *)"bit"
#define YANG_K_CASE              (const xmlChar *)"case"
#define YANG_K_CHOICE            (const xmlChar *)"choice"
#define YANG_K_CONFIG            (const xmlChar *)"config"
#define YANG_K_CONTACT           (const xmlChar *)"contact"
#define YANG_K_CONTAINER         (const xmlChar *)"container"
#define YANG_K_DEFAULT           (const xmlChar *)"default"
#define YANG_K_DESCRIPTION       (const xmlChar *)"description"
#define YANG_K_DEVIATION         (const xmlChar *)"deviation"
#define YANG_K_ENUM              (const xmlChar *)"enum"
#define YANG_K_ERROR_APP_TAG     (const xmlChar *)"error-app-tag"
#define YANG_K_ERROR_MESSAGE     (const xmlChar *)"error-message"
#define YANG_K_EXTENSION         (const xmlChar *)"extension"
#define YANG_K_FEATURE           (const xmlChar *)"feature"
#define YANG_K_FIRST             (const xmlChar *)"first"
#define YANG_K_GROUPING          (const xmlChar *)"grouping"
#define YANG_K_IDENTITY          (const xmlChar *)"identity"
#define YANG_K_IDENTITYREF       (const xmlChar *)"identityref"
#define YANG_K_IF_FEATURE        (const xmlChar *)"if-feature"
#define YANG_K_IMPORT            (const xmlChar *)"import"
#define YANG_K_INCLUDE           (const xmlChar *)"include"
#define YANG_K_INPUT             (const xmlChar *)"input"
#define YANG_K_INSERT            (const xmlChar *)"insert"
#define YANG_K_KEY               (const xmlChar *)"key"
#define YANG_K_LAST              (const xmlChar *)"last"
#define YANG_K_LEAF              (const xmlChar *)"leaf"
#define YANG_K_LEAF_LIST         (const xmlChar *)"leaf-list"
#define YANG_K_LENGTH            (const xmlChar *)"length"
#define YANG_K_LIST              (const xmlChar *)"list"
#define YANG_K_MANDATORY         (const xmlChar *)"mandatory"
#define YANG_K_MAX               (const xmlChar *)"max"
#define YANG_K_MAX_ELEMENTS      (const xmlChar *)"max-elements"
#define YANG_K_MIN               (const xmlChar *)"min"
#define YANG_K_MIN_ELEMENTS      (const xmlChar *)"min-elements"
#define YANG_K_MODULE            (const xmlChar *)"module"
#define YANG_K_MUST              (const xmlChar *)"must"
#define YANG_K_NAMESPACE         (const xmlChar *)"namespace"
#define YANG_K_NAN               (const xmlChar *)"NaN"
#define YANG_K_NEGINF            (const xmlChar *)"-INF"
#define YANG_K_NOTIFICATION      (const xmlChar *)"notification"
#define YANG_K_ORDERED_BY        (const xmlChar *)"ordered-by"
#define YANG_K_ORGANIZATION      (const xmlChar *)"organization"
#define YANG_K_OUTPUT            (const xmlChar *)"output"
#define YANG_K_PATH              (const xmlChar *)"path"
#define YANG_K_PATTERN           (const xmlChar *)"pattern"
#define YANG_K_POSITION          (const xmlChar *)"position"
#define YANG_K_POSINF            (const xmlChar *)"INF"
#define YANG_K_PREFIX            (const xmlChar *)"prefix"
#define YANG_K_PRESENCE          (const xmlChar *)"presence"
#define YANG_K_RANGE             (const xmlChar *)"range"
#define YANG_K_REFERENCE         (const xmlChar *)"reference"
#define YANG_K_REVISION          (const xmlChar *)"revision"
#define YANG_K_RPC               (const xmlChar *)"rpc"
#define YANG_K_STATUS            (const xmlChar *)"status"
#define YANG_K_SUBMODULE         (const xmlChar *)"submodule"
#define YANG_K_SYSTEM            (const xmlChar *)"system"
#define YANG_K_TYPE              (const xmlChar *)"type"
#define YANG_K_TYPEDEF           (const xmlChar *)"typedef"
#define YANG_K_UNBOUNDED         (const xmlChar *)"unbounded"
#define YANG_K_UNION             (const xmlChar *)"union"
#define YANG_K_UNIQUE            (const xmlChar *)"unique"
#define YANG_K_UNITS             (const xmlChar *)"units"
#define YANG_K_USER              (const xmlChar *)"user"
#define YANG_K_USES              (const xmlChar *)"uses"
#define YANG_K_VALUE             (const xmlChar *)"value"
#define YANG_K_WHEN              (const xmlChar *)"when"
#define YANG_K_YANG_VERSION      (const xmlChar *)"yang-version"
#define YANG_K_YIN_ELEMENT       (const xmlChar *)"yin-element"


/* used in parser routines to decide if processing can continue
 * will exit the function if critical error or continue if not
 */
#define CHK_EXIT \
    if (res != NO_ERR) { \
	if (res < ERR_LAST_SYS_ERR || res==ERR_NCX_EOF) { \
	    return res; \
	} else { \
	    retres = res; \
	} \
    }

/* used in parser routines to decide if processing can continue
 * does not return, just evaluates to TRUE if there is
 * a critical error and false if processing can continue
 */
#define NEED_EXIT					\
    ((res == NO_ERR) ? FALSE :				\
     (res < ERR_LAST_SYS_ERR || res==ERR_NCX_EOF) ?	\
     TRUE : FALSE)

#endif	    /* _H_yangconst */
