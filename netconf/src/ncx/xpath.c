/*  FILE: xpath.c

    Schema and data model Xpath search support
		
*********************************************************************
*                                                                   *
*                  C H A N G E   H I S T O R Y                      *
*                                                                   *
*********************************************************************

date         init     comment
----------------------------------------------------------------------
31dec07      abb      begun

*********************************************************************
*                                                                   *
*                     I N C L U D E    F I L E S                    *
*                                                                   *
*********************************************************************/
#include  <stdio.h>
#include  <stdlib.h>
#include  <memory.h>

#include <xmlstring.h>

#ifndef _H_procdefs
#include  "procdefs.h"
#endif

#ifndef _H_def_reg
#include "def_reg.h"
#endif

#ifndef _H_dlq
#include "dlq.h"
#endif

#ifndef _H_grp
#include "grp.h"
#endif

#ifndef _H_ncxconst
#include "ncxconst.h"
#endif

#ifndef _H_ncx
#include "ncx.h"
#endif

#ifndef _H_obj
#include "obj.h"
#endif

#ifndef _H_tk
#include "tk.h"
#endif

#ifndef _H_typ
#include "typ.h"
#endif

#ifndef _H_xpath
#include "xpath.h"
#endif

#ifndef _H_yangconst
#include "yangconst.h"
#endif


/********************************************************************
*                                                                   *
*                       C O N S T A N T S                           *
*                                                                   *
*********************************************************************/


/********************************************************************
*                                                                   *
*                         V A R I A B L E S                         *
*                                                                   *
*********************************************************************/



/********************************************************************
* FUNCTION do_errmsg
* 
* Generate the errormsg
*
* INPUTS:
*    tkc == token chain in progress (may be NULL)
*    mod == module in progress
*    errtk == error tk_token_t to temp. set
*    res == error enumeration
*
*********************************************************************/
static void
    do_errmsg (tk_chain_t *tkc,
	       ncx_module_t *mod,
	       tk_token_t *errtk,
	       status_t  res)
{
    tk_token_t  *savetk;

    if (tkc) {
	savetk = tkc->cur;
	tkc->cur = errtk;
    }
    ncx_print_errormsg(tkc, mod, res);
    if (tkc) {
	tkc->cur = savetk;
    }
}  /* do_errmsg */


/********************************************************************
* FUNCTION next_nodeid
* 
* Get the next Name of QName segment of an Xpath schema-nodeid
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    tkc == token chain in progress  (may be NULL)
*    mod == module in progress
*    obj == object calling this fn (for error purposes)
*    target == Xpath expression string to evaluate
*    prefix == buffer to store prefix portion of QName (if any)
*    name == buffer to store name portion of segment
*    len == address of return byte count
*
* OUTPUTS:
*   prefix[] == prefix portion of QName
*   name[] == name portion of QName
*   *cnt == number of bytes used in target
*
* RETURNS:
*    status
*********************************************************************/
static status_t
    next_nodeid (tk_chain_t *tkc,
		 ncx_module_t *mod,
		 obj_template_t *obj,
		 const xmlChar *target,
		 xmlChar *prefix,
		 xmlChar *name,
		 uint32 *len)
{
    const xmlChar *p, *q;
    uint32         cnt;
    status_t       res;

    /* find the EOS or a separator */
    p = target;
    while (*p && *p != ':' && *p != '/') {
	p++;
    }
    cnt = (uint32)(p-target);

    if (!ncx_valid_name(target, cnt)) {
	xml_strncpy(prefix, target, min(cnt, NCX_MAX_NLEN));
	log_error("\nError: invalid name string (%s)", prefix);
	res = ERR_NCX_INVALID_NAME;
	do_errmsg(tkc, mod, obj->tk, res);
	return res;
    }

    if (*p==':') {
	/* copy prefix, then get name portion */
	xml_strncpy(prefix, target, cnt);

	q = ++p;
	while (*q && *q != '/') {
	    q++;
	}
	cnt = (uint32)(q-p);

	if (!ncx_valid_name(p, cnt)) {
	    xml_strncpy(name, p, min(cnt, NCX_MAX_NLEN));
	    log_error("\nError: invalid name string (%s)", name);
	    res = ERR_NCX_INVALID_NAME;
	    do_errmsg(tkc, mod, obj->tk, res);
	    return res;
	}

	xml_strncpy(name, p, cnt);
	*len = (uint32)(q-target);
    } else  {
	/* found EOS or pathsep, got just one 'name' string */
	xml_strncpy(name, target, cnt);
	*prefix = '\0';
	*len = cnt;
    }
    return NO_ERR;

}  /* next_nodeid */


/********************************************************************
* FUNCTION next_nodeid_noerr
* 
* Get the next Name of QName segment of an Xpath schema-nodeid
*
* Error messages are not printed by this function!!
*
* INPUTS:
*    target == Xpath expression string in progress to evaluate
*    prefix == buffer to store prefix portion of QName (if any)
*    name == buffer to store name portion of segment
*    len == address of return byte count
*
* OUTPUTS:
*   prefix[] == prefix portion of QName
*   name[] == name portion of QName
*   *cnt == number of bytes used in target
*
* RETURNS:
*    status
*********************************************************************/
static status_t
    next_nodeid_noerr (const xmlChar *target,
		       xmlChar *prefix,
		       xmlChar *name,
		       uint32 *len)
{
    const xmlChar *p, *q;
    uint32         cnt;

    /* find the EOS or a separator */
    p = target;
    while (*p && *p != ':' && *p != '/') {
	p++;
    }
    cnt = (uint32)(p-target);

    if (!ncx_valid_name(target, cnt)) {
	return ERR_NCX_INVALID_NAME;
    }

    if (*p==':') {
	/* copy prefix, then get name portion */
	xml_strncpy(prefix, target, cnt);

	q = ++p;
	while (*q && *q != '/') {
	    q++;
	}
	cnt = (uint32)(q-p);

	if (!ncx_valid_name(p, cnt)) {
	    return ERR_NCX_INVALID_NAME;
	}

	xml_strncpy(name, p, cnt);
	*len = (uint32)(q-target);
    } else  {
	/* found EOS or pathsep, got just one 'name' string */
	xml_strncpy(name, target, cnt);
	*prefix = '\0';
	*len = cnt;
    }
    return NO_ERR;

}  /* next_nodeid_noerr */


/********************************************************************
* FUNCTION next_val_nodeid
* 
* Get the next Name of QName segment of an Xpath schema-nodeid
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    target == Xpath expression string to evaluate
*    prefix == buffer to store prefix portion of QName (if any)
*    name == buffer to store name portion of segment
*    len == address of return byte count
*
* OUTPUTS:
*   prefix[] == prefix portion of QName
*   name[] == name portion of QName
*   *cnt == number of bytes used in target
*
* RETURNS:
*    status
*********************************************************************/
static status_t
    next_val_nodeid (const xmlChar *target,
		     xmlChar *prefix,
		     xmlChar *name,
		     uint32 *len)
{
    const xmlChar *p, *q;
    uint32         cnt;

    /* find the EOS or a separator */
    p = target;
    while (*p && *p != ':' && *p != '/') {
	p++;
    }
    cnt = (uint32)(p-target);

    if (!ncx_valid_name(target, cnt)) {
	xml_strncpy(prefix, target, min(cnt, NCX_MAX_NLEN));
	log_error("\nError: invalid name string (%s)", prefix);
	return ERR_NCX_INVALID_NAME;
    }

    if (*p==':') {
	/* copy prefix, then get name portion */
	xml_strncpy(prefix, target, cnt);

	q = ++p;
	while (*q && *q != '/') {
	    q++;
	}
	cnt = (uint32)(q-p);

	if (!ncx_valid_name(p, cnt)) {
	    xml_strncpy(name, p, min(cnt, NCX_MAX_NLEN));
	    log_error("\nError: invalid name string (%s)", name);
	    return ERR_NCX_INVALID_NAME;
	}

	xml_strncpy(name, p, cnt);
	*len = (uint32)(q-target);
    } else  {
	/* found EOS or pathsep, got just one 'name' string */
	xml_strncpy(name, target, cnt);
	*prefix = '\0';
	*len = cnt;
    }
    return NO_ERR;

}  /* next_val_nodeid */


/********************************************************************
* FUNCTION find_schema_node
* 
* Follow the absolute-path or descendant node path expression
* and return the obj_template_t that it indicates, and the
* que that the object is in
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    tkc == token chain in progress
*    mod == module in progress
*    obj == object calling this fn (for error purposes)
*    datadefQ == Q of obj_template_t containing 'obj'
*    target == Xpath expression string to evaluate
*    targobj == address of return object  (may be NULL)
*    targQ == address of return target queue (may be NULL)
*    errtk == error token to use
*
* OUTPUTS:
*   if non-NULL inputs:
*      *targobj == target object
*      *targQ == datadefQ header for targobj
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    find_schema_node (tk_chain_t *tkc,
		      ncx_module_t *mod,
		      obj_template_t *obj,
		      dlq_hdr_t *datadefQ,
		      const xmlChar *target,
		      obj_template_t **targobj,
		      dlq_hdr_t **targQ,
		      tk_token_t *errtk)
{
    ncx_import_t   *imp;
    ncx_module_t   *impmod;
    obj_template_t *curobj, *nextobj;
    dlq_hdr_t      *curQ;
    const xmlChar  *str;
    uint32          len;
    status_t        res;
    ncx_node_t      dtyp;
    xmlChar         prefix[NCX_MAX_NLEN+1];
    xmlChar         name[NCX_MAX_NLEN+1];

    imp = NULL;
    impmod = NULL;
    dtyp = NCX_NT_OBJ;

    /* skip the first fwd slash, if any */
    if (*target == '/') {
	str = ++target;
    } else {
	str = target;
    }

    /* get the first QName (prefix, name) */
    res = next_nodeid(tkc, mod, obj, str, prefix, name, &len);
    if (res != NO_ERR) {
	return res;
    } else {
	str += len;
    }

    /* get the import if there is a real prefix entered */
    if (*prefix && xml_strcmp(prefix, mod->prefix)) {
	imp = ncx_find_pre_import(mod, prefix);
	if (!imp) {
	    log_error("\nError: prefix '%s' not found in module imports"
		      " in Xpath target %s", prefix, target);
	    res = ERR_NCX_INVALID_NAME;
	    do_errmsg(tkc, mod, errtk, res);
	    return res;
	}
	impmod = ncx_find_module(imp->module);
	if (!impmod) {
	    log_error("\nError: module '%s' not found for prefix %s"
		      " in Xpath target %s",
		      imp->module, prefix, target);
	    res = ERR_NCX_MOD_NOT_FOUND;
	    do_errmsg(tkc, mod, errtk, res);
	    return res;
	}
    }

    /* get the first object template */
    if (imp) {
	curobj = ncx_locate_modqual_import(imp->module, name,
					   mod->diffmode, &dtyp);
    } else if (*target == '/') {
	curobj = obj_find_template_top(mod, mod->name, name);
    } else {
	curobj = obj_find_template(datadefQ, mod->name, name);
    }

    if (!curobj) {
	if (ncx_valid_name2(name)) {
	    res = ERR_NCX_DEF_NOT_FOUND;
	} else {
	    res = ERR_NCX_INVALID_NAME;
	}
	log_error("\nError: object '%s' not found in module %s"
		  " in Xpath target %s",
		  name, (imp) ? imp->module : mod->name,
		  target);
	do_errmsg(tkc, mod, errtk, res);
	return res;
    } else {
	curQ = datadefQ;
    }

    if (obj_is_augclone(curobj)) {
	res = ERR_NCX_INVALID_VALUE;
	log_error("\nError: augment is external: node '%s'"
		  " from module %s, line %u in Xpath target %s",
		  name, curobj->mod->name,
		  curobj->tk->linenum, target);
	do_errmsg(tkc, mod, errtk, res);
	return res;
    }

    /* got the first object; keep parsing node IDs
     * until the Xpath expression is done or an error occurs
     */
    while (*str == '/') {
	str++;
	/* get the next QName (prefix, name) */
	res = next_nodeid(tkc, mod, obj, str, prefix, name, &len);
	if (res != NO_ERR) {
	    return res;
	} else {
	    str += len;
	}

	/* make sure the prefix is valid, if present */
	if (*prefix && xml_strcmp(prefix, mod->prefix)) {
	    imp = ncx_find_pre_import(mod, prefix);
	    if (!imp) {
		log_error("\nError: prefix '%s' not found in module"
			  " imports in Xpath target %s",
			  prefix, target);
		res = ERR_NCX_INVALID_NAME;
		do_errmsg(tkc, mod, errtk, res);
		return res;
	    }
	} else {
	    imp = NULL;
	}

	/* make sure the name is a valid name string */
	if (!ncx_valid_name2(name)) {
	    log_error("\nError: object name '%s' not a valid "
		      "identifier in Xpath target %s",
		      name, target);
	    res = ERR_NCX_INVALID_NAME;
	    do_errmsg(tkc, mod, errtk, res);
	    return res;
	}

	/* determine 'nextval' based on [curval, prefix, name] */
	curQ = obj_get_datadefQ(curobj);

	if (curQ) {
	    nextobj = obj_find_template(curQ,
					(imp) ? imp->module : 
					mod->name, name);
	} else {
	    res = ERR_NCX_DEFSEG_NOT_FOUND;
	    log_error("\nError: '%s' in Xpath target '%s' invalid: "
		      "%s on line %u is a %s",
		      name, target, obj_get_name(curobj),
		      curobj->tk->linenum, obj_get_typestr(curobj));
	    do_errmsg(tkc, mod, errtk, res);
	    return res;
	}

	if (nextobj) {
	    curobj = nextobj;
	} else {
	    res = ERR_NCX_DEFSEG_NOT_FOUND;
	    log_error("\nError: object '%s' not found in module %s",
		      name, (imp) ? imp->module : mod->name);
	    do_errmsg(tkc, mod, errtk, res);
	    return res;
	}
    }

    if (targobj) {
	*targobj = curobj;
    }
    if (targQ) {
	*targQ = curQ;
    }

    return NO_ERR;

}  /* find_schema_node */


/********************************************************************
* FUNCTION find_schema_node_int
* 
* Follow the absolute-path expression
* and return the obj_template_t that it indicates
* A missing prefix means check any namespace for the symbol
*
* !!! Internal version !!!
* !!! Error messages are not printed by this function!!
*
* INPUTS:
*    target == Absolute Xpath expression string to evaluate
*    targobj == address of return object  (may be NULL)
*
* OUTPUTS:
*   if non-NULL inputs:
*      *targobj == target object
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    find_schema_node_int (const xmlChar *target,
			  obj_template_t **targobj)
{
    obj_template_t *curobj, *nextobj;
    dlq_hdr_t      *curQ;
    ncx_module_t   *mod;
    const xmlChar  *str, *modname;
    uint32          len;
    status_t        res;
    ncx_node_t      dtyp;
    xmlChar         prefix[NCX_MAX_NLEN+1];
    xmlChar         name[NCX_MAX_NLEN+1];


    /* skip the first fwd slash, if any
     * the target must be from the config root
     * so if the first fwd-slash is missing then
     * just keep going and assume the config root anyway
     */
    if (*target == '/') {
	str = ++target;
    } else {
	str = target;
    }

    /* get the first QName (prefix, name) */
    res = next_nodeid_noerr(str, prefix, name, &len);
    if (res != NO_ERR) {
	return res;
    } else {
	str += len;
    }

    /* get the import if there is a real prefix entered */
    if (*prefix) {
	mod = def_reg_find_module_prefix(prefix);
	if (!mod) {
	    return ERR_NCX_INVALID_NAME;
	}
	/* get the first object template */
	curobj = obj_find_template_top(mod, mod->name, name);
    } else {
	/* no prefix given, check all top-level objects */
	dtyp = NCX_NT_OBJ;
        curobj = (obj_template_t *)
	    def_reg_find_any_moddef(&modname, name, &dtyp);
    }

    /* check if first level object found */
    if (!curobj) {
	if (ncx_valid_name2(name)) {
	    res = ERR_NCX_DEF_NOT_FOUND;
	} else {
	    res = ERR_NCX_INVALID_NAME;
	}
	return res;
    }

    if (obj_is_augclone(curobj)) {
	return ERR_NCX_INVALID_VALUE;
    }

    /* got the first object; keep parsing node IDs
     * until the Xpath expression is done or an error occurs
     */
    while (*str == '/') {
	str++;
	/* get the next QName (prefix, name) */
	res = next_nodeid_noerr(str, prefix, name, &len);
	if (res != NO_ERR) {
	    return res;
	} else {
	    str += len;
	}

	/* make sure the name is a valid name string */
	if (!ncx_valid_name2(name)) {
	    return ERR_NCX_INVALID_NAME;
	}

	/* determine 'nextval' based on [curval, prefix, name] */
	curQ = obj_get_datadefQ(curobj);
	if (!curQ) {
	    return ERR_NCX_DEFSEG_NOT_FOUND;
	}

	/* make sure the prefix is valid, if present */
	if (*prefix) {
	    mod = def_reg_find_module_prefix(prefix);
	    if (!mod) {
		return ERR_NCX_INVALID_NAME;
	    }
	    nextobj = obj_find_template(curQ, mod->name, name);
	} else {
	    /* no prefix given; try current module first */
	    nextobj = obj_find_template(curQ, obj_get_mod_name(curobj), 
					name); 
	    if (!nextobj) {
		nextobj = obj_find_template(curQ, NULL, name); 
	    }
	}

	/* setup next loop or error exit because last node not found */
	if (nextobj) {
	    curobj = nextobj;
	} else {
	    return ERR_NCX_DEFSEG_NOT_FOUND;
	}
    }

    if (targobj) {
	*targobj = curobj;
    }

    return NO_ERR;

}  /* find_schema_node_int */


/********************************************************************
* FUNCTION get_curmod_from_prefix
* 
* Get the correct module to use for a given prefix
*
* INPUTS:
*    prefix == string to check
*    mod == module to use for the default context
*           and prefixes will be relative to this module's
*           import statements.
*        == NULL and the default registered prefixes
*           will be used
*    targmod == address of return module
*
* OUTPUTS:
*    *targmod == target moduke to use
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    get_curmod_from_prefix (const xmlChar *prefix,
			    ncx_module_t *mod,
			    ncx_module_t **targmod)
{
    ncx_import_t   *imp;
    status_t        res;

    /* get the import if there is a real prefix entered */
    if (prefix && *prefix) {
	if (!mod) {
	    *targmod = def_reg_find_module_prefix(prefix);
	    if (!*targmod) {
		res = ERR_NCX_MOD_NOT_FOUND;
	    }
	} else if (xml_strcmp(prefix, mod->prefix)) {
	    imp = ncx_find_pre_import(mod, prefix);
	    if (!imp) {
		res = ERR_NCX_INVALID_NAME;
	    }
	    *targmod = ncx_find_module(imp->module);
	    if (!*targmod) {
		res = ERR_NCX_MOD_NOT_FOUND;
	    }
	} else {
	    *targmod = mod;
	    res = NO_ERR;
	}
    } else if (mod) {
	*targmod = mod;
	res = NO_ERR;
    } else {
	*targmod = NULL;
	res = ERR_NCX_DATA_MISSING;
    }
    return res;

}   /* get_curmod_from_prefix */


/********************************************************************
* FUNCTION find_val_node
* 
* Follow the Xpath expression
* and return the val_value_t that it indicates, if any
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    startval == val_value_t node to start search
*    mod == module to use for the default context
*           and prefixes will be relative to this module's
*           import statements.
*        == NULL and the default registered prefixes
*           will be used
*    target == Xpath expression string to evaluate
*    targval == address of return value  (may be NULL)
*
* OUTPUTS:
*   if non-NULL inputs:
*      *targval == target value node
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    find_val_node (val_value_t *startval,
		   ncx_module_t *mod,
		   const xmlChar *target,
		   val_value_t **targval)
{
    val_value_t    *curval;
    ncx_module_t   *usemod;
    const xmlChar  *str;
    uint32          len;
    status_t        res;
    xmlChar         prefix[NCX_MAX_NLEN+1];
    xmlChar         name[NCX_MAX_NLEN+1];

    /* check absolute path starting with root val */
    if (*target == '/' && !obj_is_root(startval->obj)) {
	log_error("\nError: Absolute path given but startval is not "
		  "root in Xpath target %s", target);
	return ERR_NCX_DEF_NOT_FOUND;
    }

    /* check '/' corner-case */
    if (!xml_strcmp(target, (const xmlChar *)"/")) {
	if (targval) {
	    *targval = startval;
	}
	return NO_ERR;
    }

    /* skip the first fwd slash, if any */
    if (*target == '/') {
	str = ++target;
    } else {
	str = target;
    }

    /* get the first QName (prefix, name) */
    res = next_val_nodeid(str, prefix, name, &len);
    if (res != NO_ERR) {
	return res;
    } else {
	str += len;
    }

    res = get_curmod_from_prefix(prefix, mod, &usemod);
    if (res != NO_ERR) {
	if (*prefix) {
	    log_error("\nError: module not found for prefix %s"
		      " in Xpath target %s",
		      prefix, target);
	    return ERR_NCX_MOD_NOT_FOUND;
	} else {
	    log_error("\nError: no module prefix specified"
			  " in Xpath target %s", target);
	    return ERR_NCX_MOD_NOT_FOUND;
	}
    }

    /* get the first value node */
    curval = val_find_child(startval, usemod->name, name);
    if (!curval) {
	if (ncx_valid_name2(name)) {
	    res = ERR_NCX_DEF_NOT_FOUND;
	} else {
	    res = ERR_NCX_INVALID_NAME;
	}
	log_error("\nError: value node '%s' not found for module %s"
		  " in Xpath target %s",
		  name, usemod->name, target);
	return res;
    }

    /* got the first object; keep parsing node IDs
     * until the Xpath expression is done or an error occurs
     */
    while (*str == '/') {
	str++;
	/* get the next QName (prefix, name) */
	res = next_val_nodeid(str, prefix, name, &len);
	if (res != NO_ERR) {
	    return res;
	} else {
	    str += len;
	}

	res = get_curmod_from_prefix(prefix, mod, &usemod);
	if (res != NO_ERR) {
	    if (*prefix) {
		log_error("\nError: module not found for prefix %s"
			  " in Xpath target %s",
			  prefix, target);
		return ERR_NCX_MOD_NOT_FOUND;
	    } else {
		log_error("\nError: no module prefix specified"
			  " in Xpath target %s", target);
		return ERR_NCX_MOD_NOT_FOUND;
	    }
	}

	/* determine 'nextval' based on [curval, prefix, name] */
	switch (curval->obj->objtype) {
	case OBJ_TYP_CONTAINER:
	case OBJ_TYP_LIST:
	case OBJ_TYP_CHOICE:
	case OBJ_TYP_CASE:
	case OBJ_TYP_RPC:
	case OBJ_TYP_RPCIO:
	case OBJ_TYP_NOTIF:
	    curval = val_find_child(curval, usemod->name, name);
	    if (!curval) {
		if (ncx_valid_name2(name)) {
		    res = ERR_NCX_DEF_NOT_FOUND;
		} else {
		    res = ERR_NCX_INVALID_NAME;
		}
		log_error("\nError: value node '%s' not found for module %s"
			  " in Xpath target %s",
			  name, usemod->name, target);
		return res;
	    }
	    break;
	case OBJ_TYP_LEAF:
	case OBJ_TYP_LEAF_LIST:
	    res = ERR_NCX_DEFSEG_NOT_FOUND;
	    log_error("\nError: '%s' in Xpath target '%s' invalid: "
		      "%s is a %s",
		      name, target, curval->name,
		      obj_get_typestr(curval->obj));
	    return res;
	default:
	    res = SET_ERROR(ERR_INTERNAL_VAL);
	    do_errmsg(NULL, mod, NULL, res);
	    return res;
	}
    }

    if (targval) {
	*targval = curval;
    }
    return NO_ERR;

}  /* find_val_node */


/********************************************************************
* FUNCTION parse_token
* 
* Parse the keyref token sequence for a specific token type
* It has already been tokenized
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    pcb == parser control block in progress
*    tktyp == expected token type
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    parse_token (xpath_pcb_t *pcb,
		 tk_type_t  tktype)
{
    status_t     res;

    /* get the next token */
    res = TK_ADV(pcb->tkc);
    if (res != NO_ERR) {
	ncx_print_errormsg(pcb->tkc, pcb->mod, res);
	return res;
    }

    if (TK_CUR_TYP(pcb->tkc) != tktype) {
	res = ERR_NCX_WRONG_TKTYPE;
	ncx_mod_exp_err(pcb->tkc, pcb->mod, res,
			tk_get_token_name(tktype));
	return res;
    }

    return NO_ERR;

}  /* parse_token */


/********************************************************************
* FUNCTION set_next_objnode
* 
* Get the object identifier associated with
* QName in an keyref XPath expression
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    pcb == parser control block in progress
*    prefix == prefix value used if any
*    nodename == node name to find (must be present)
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    set_next_objnode (xpath_pcb_t *pcb,
		      const xmlChar *prefix,
		      const xmlChar *nodename)
{
    const obj_template_t  **useobj, *foundobj;
    ncx_module_t           *targmod;
    status_t                res;

    foundobj = NULL;
    res = NO_ERR;

    switch (pcb->curmode) {
    case XP_CM_TARGET:
	useobj = &pcb->targobj;
	break;
    case XP_CM_ALT:
	useobj = &pcb->altobj;
	break;
    case XP_CM_KEYVAR:
	useobj = &pcb->varobj;
	break;
    default:
	res = SET_ERROR(ERR_INTERNAL_VAL);
    }

    if (res == NO_ERR) {
	res = get_curmod_from_prefix(prefix,
				     pcb->objmod,
				     &targmod);
	if (res != NO_ERR) {
	    log_error("\nError: Module for prefix '%s' not found",
		      (prefix) ? prefix : (const xmlChar *)"");
	}
    }

    if (res != NO_ERR) {
	ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
	return res;
    }

    if (*useobj) {
	if (obj_is_root(*useobj)) {
	    foundobj = 
		obj_find_template_top(targmod,
				      ncx_get_modname(targmod),
				      nodename);
	} else {
	    /* get child node of this object */
	    foundobj = 
		obj_find_child(*useobj,
			       ncx_get_modname(targmod),
			       nodename);
	}
    } else if (pcb->curmode == XP_CM_KEYVAR ||
	       pcb->curmode == XP_CM_ALT) {

	/* setting object for the first time
	 * get child node of the current context object
	 */
	if (pcb->targobj) {
	    foundobj = 
		obj_find_child(pcb->targobj,
			       ncx_get_modname(targmod),
			       nodename);
	}
    } else if (pcb->abspath) {
	/* setting object for the first time
	 * get top-level object from object module 
	 */
	foundobj = 
	    obj_find_template_top(targmod,
				  ncx_get_modname(targmod),
				  nodename);
    } else {
	/* setting object for the first time
	 * but the context node is a leaf, so there
	 * is no possible child node of the the start object 
	 */
	;
    }

    if (!foundobj) {
	res = ERR_NCX_DEF_NOT_FOUND;
	if (prefix) {
	    log_error("\nError: object not found '%s:%s'",
		      prefix, nodename);
	} else {
	    log_error("\nError: object not found '%s'",
		      nodename);
	}
	ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
    } else {
	*useobj = foundobj;
    }

    return res;

} /* set_next_objnode */


/********************************************************************
* FUNCTION move_up_obj
* 
* Get the parent object identifier
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    pcb == parser control block in progress
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    move_up_obj (xpath_pcb_t *pcb)
{
    const obj_template_t  **useobj, *foundobj;
    status_t                res;

    foundobj = NULL;
    res = NO_ERR;

    switch (pcb->curmode) {
    case XP_CM_TARGET:
	useobj = &pcb->targobj;
	break;
    case XP_CM_ALT:
	useobj = &pcb->altobj;
	break;
    default:
	res = SET_ERROR(ERR_INTERNAL_VAL);
	ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
	return res;
    }

    if (*useobj) {
	/* get child node of this object */
	if ((*useobj)->parent) {
	    foundobj = (*useobj)->parent;
	} else if (*useobj != pcb->docroot) {
	    foundobj = pcb->docroot;
	}
    } else {
	/* setting object for the first time
	 * get parent node of the the start object 
	 */
	foundobj = pcb->obj->parent;
	if (!foundobj) {
	    foundobj = pcb->docroot;
	}
    }

    if (!foundobj) {
	res = ERR_NCX_DEF_NOT_FOUND;
	log_error("\nError: parent not found for object '%s'",
		  obj_get_name(*useobj));
	ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
    } else {
	*useobj = foundobj;
    }

    return res;

} /* move_up_obj */


/********************************************************************
* FUNCTION parse_node_identifier
* 
* Parse the keyref node-identifier string
* It has already been tokenized
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* node-identifier        = [prefix ":"] identifier
*
* INPUTS:
*    pcb == parser control block in progress
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    parse_node_identifier (xpath_pcb_t *pcb)
{
    const xmlChar  *prefix;
    ncx_import_t   *import;
    status_t        res;

    prefix = NULL;
    import = NULL;
    res = NO_ERR;

    /* get the next token in the step, node-identifier */
    res = TK_ADV(pcb->tkc);
    if (res != NO_ERR) {
	ncx_print_errormsg(pcb->tkc, pcb->mod, res);
	return res;
    }

    switch (TK_CUR_TYP(pcb->tkc)) {
    case TK_TT_MSTRING:
	/* pfix:identifier */
	prefix = TK_CUR_MOD(pcb->tkc);
	if (xml_strcmp(pcb->mod->prefix, prefix)) {
	    import = ncx_find_pre_import(pcb->mod, prefix);
	    if (!import) {
		res = ERR_NCX_PREFIX_NOT_FOUND;
		log_error("\nError: import for prefix '%s' not found",
			  prefix);
		ncx_print_errormsg(pcb->tkc, pcb->mod, res);
		break;
	    }
	}
	/* fall through to check QName */
    case TK_TT_TSTRING:
	if (pcb->obj) {
	    res = set_next_objnode(pcb, prefix,
				   TK_CUR_VAL(pcb->tkc));
	} /* else identifier not checked here */
	break;
    default:
	res = ERR_NCX_WRONG_TKTYPE;
	ncx_mod_exp_err(pcb->tkc, pcb->mod, res,
			tk_get_token_name(TK_CUR_TYP(pcb->tkc)));
    }

    return res;

}  /* parse_node_identifier */


/********************************************************************
* FUNCTION parse_current_fn
* 
* Parse the keyref current-function token sequence
* It has already been tokenized
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* current-function-invocation = 'current()'
*
* INPUTS:
*    pcb == parser control block in progress
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    parse_current_fn (xpath_pcb_t *pcb)
{
    status_t     res;

    /* get the function name 'current' */
    res = parse_token(pcb, TK_TT_TSTRING);
    if (res != NO_ERR) {
	return res;
    }
    if (xml_strcmp(TK_CUR_VAL(pcb->tkc),
		   (const xmlChar *)"current")) {
	res = ERR_NCX_WRONG_VAL;
	ncx_mod_exp_err(pcb->tkc, pcb->mod, res,
			"current() function");
	return res;
    }

    /* get the left paren '(' */
    res = parse_token(pcb, TK_TT_LPAREN);
    if (res != NO_ERR) {
	return res;
    }

    /* get the right paren ')' */
    res = parse_token(pcb, TK_TT_RPAREN);
    if (res != NO_ERR) {
	return res;
    }

    /* assign the context node to the start object node */
    if (pcb->obj) {
	switch (pcb->curmode) {
	case XP_CM_TARGET:
	    pcb->targobj = pcb->obj;
	    break;
	case XP_CM_ALT:
	    pcb->altobj = pcb->obj;
	    break;
	default:
	    res = SET_ERROR(ERR_INTERNAL_VAL);
	}
    }

    return NO_ERR;

}  /* parse_current_fn */


/********************************************************************
* FUNCTION parse_path_key_expr
* 
* Parse the keyref *path-key-expr token sequence
* It has already been tokenized
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* path-key-expr          = current-function-invocation "/"
*                          rel-path-keyexpr
*
* rel-path-keyexpr       = 1*(".." "/") *(node-identifier "/")
*                          node-identifier
*
* INPUTS:
*    pcb == parser control block in progress
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    parse_path_key_expr (xpath_pcb_t *pcb)
{
    tk_type_t    nexttyp;
    status_t     res;
    boolean      done;

    res = NO_ERR;
    done = FALSE;

    pcb->altobj = NULL;
    pcb->curmode = XP_CM_ALT;

    /* get the current function call 'current()' */
    res = parse_current_fn(pcb);
    if (res != NO_ERR) {
	return res;
    }

    /* get the path separator '/' */
    res = parse_token(pcb, TK_TT_FSLASH);
    if (res != NO_ERR) {
	return res;
    }

    /* make one loop for each step of the first part of
     * the rel-path-keyexpr; the '../' token pairs
     */
    while (!done) {
	/* get the parent marker '..' */
	res = parse_token(pcb, TK_TT_RANGESEP);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	/* get the path separator '/' */
	res = parse_token(pcb, TK_TT_FSLASH);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	/* move the context node up one level */
	if (pcb->obj) {
	    res = move_up_obj(pcb);
	    if (res != NO_ERR) {
		done = TRUE;
		continue;
	    }
	}

	/* check the next token;
	 * it may be the start of another '../' pair
	 */
	nexttyp = tk_next_typ(pcb->tkc);
	if (nexttyp != TK_TT_RANGESEP) {
	    done = TRUE;
	} /* else keep going to the next '../' pair */
    }

    if (res != NO_ERR) {
	return res;
    }

    /* get the node-identifier sequence */
    done = FALSE;
    while (!done) {
	res = parse_node_identifier(pcb);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	/* check the next token;
	 * it may be the fwd slash to start another step
	 */
	nexttyp = tk_next_typ(pcb->tkc);
	if (nexttyp != TK_TT_FSLASH) {
	    done = TRUE;
	} else {
	    res = parse_token(pcb, TK_TT_FSLASH);
	    if (res != NO_ERR) {
		done = TRUE;
	    }
	}
    }

    return res;

}  /* parse_path_key_expr */


/********************************************************************
* FUNCTION get_key_number
* 
* Get the key number for the specified key
* It has already been tokenized
*
* INPUTS:
*    obj == list object to check
*    keyobj == key leaf object to find 
*
* RETURNS:
*   int32   [0..N-1] key number or -1 if not a key in this obj
*********************************************************************/
static int32
    get_key_number (const obj_template_t *obj,
		    const obj_template_t *keyobj)
{
    const obj_key_t       *key;
    int32                  keynum;

    keynum = 0;

    key = obj_first_ckey(obj);
    while (key) {
	if (key->keyobj == keyobj) {
	    return keynum;
	}

	keynum++;
	key = obj_next_ckey(key);
    }

    return -1;

} /* get_key_number */


/********************************************************************
* FUNCTION parse_path_predicate
* 
* Parse the keyref *path-predicate token sequence
* It has already been tokenized
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* path-predicate         = "[" *WSP path-equality-expr *WSP "]"
*
* path-equality-expr     = node-identifier *WSP "=" *WSP path-key-expr
*
* path-key-expr          = current-function-invocation "/"
*                          rel-path-keyexpr
*
* node-identifier        = [prefix ":"] identifier
*
* current-function-invocation = 'current()'
*
* INPUTS:
*    pcb == parser control block in progress
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    parse_path_predicate (xpath_pcb_t *pcb)
{
    tk_type_t              nexttyp;
    status_t               res;
    boolean                done;
    uint64                 keyflags, keybit;
    uint32                 keytotal, keycount, loopcount;
    int32                  keynum;

    res = NO_ERR;
    done = FALSE;
    keyflags = 0;
    keytotal = 0;
    keycount = 0;
    loopcount = 0;

    if (pcb->targobj && pcb->targobj->objtype == OBJ_TYP_LIST) {
	keytotal = obj_key_count(pcb->targobj);
	if (keytotal > 64) {
	    log_warn("\nWarning: Only first 64 keys in list '%s'"
		     " can be checked in XPath expression", 
		     obj_get_name(pcb->obj));
	}
    }
    
    /* make one loop for each step */
    while (!done) {

	/* only used if pcb->obj set and validating expr */
	loopcount++;

	/* get the left bracket '[' */
	res = parse_token(pcb, TK_TT_LBRACK);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	pcb->varobj = NULL;
	pcb->curmode = XP_CM_KEYVAR;

	/* get the node-identifier next */
	res = parse_node_identifier(pcb);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	/* validate the variable object foo in [foo = expr] */
	if (pcb->obj && pcb->varobj) {
	    if (pcb->varobj->objtype != OBJ_TYP_LEAF) {
		res = ERR_NCX_WRONG_TYPE;
		log_error("\nError: path predicate found is %s '%s'",
			  obj_get_typestr(pcb->varobj),
			  obj_get_name(pcb->varobj));
		ncx_mod_exp_err(pcb->tkc, 
				pcb->objmod, 
				res, "leaf");
		done = TRUE;
		continue;
	    }

	    switch (pcb->source) {
	    case XP_SRC_KEYREF:
		if (!obj_is_key(pcb->varobj)) {
		    res = ERR_NCX_TYPE_NOT_INDEX;
		    log_error("\nError: path predicate '%s' is "
			      "not a key leaf",
			      obj_get_name(pcb->varobj));
		    ncx_mod_exp_err(pcb->tkc, 
				    pcb->objmod, 
				    res, "key leaf");
		    done = TRUE;
		    continue;
		}
		break;
	    case XP_SRC_LEAFREF:
		break;
	    default:
		res = SET_ERROR(ERR_INTERNAL_VAL);
		ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
		done = TRUE;
		continue;
	    }

	    keynum = get_key_number(pcb->targobj, pcb->varobj);
	    if (keynum == -1) {
		SET_ERROR(ERR_INTERNAL_VAL);
	    } else if (keynum < 64) {
		keybit = (uint64)(1 << keynum);
		if (keyflags & keybit) {
		    res = ERR_NCX_EXTRA_PARM;
		    log_error("\nError: key '%s' already specified "
			      "in XPath expression for object '%s'",
			      obj_get_name(pcb->varobj),
			      obj_get_name(pcb->targobj));
		    ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
		} else {
		    keycount++;
		    keyflags |= keybit;
		}
	    } else {
		log_warn("\nWarning: Key '%s' skipped in validation test",
			 obj_get_name(pcb->varobj));
	    }
	} 

	/* get the equals sign '=' */
	res = parse_token(pcb, TK_TT_EQUAL);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	/* get the path-key-expr next */
	res = parse_path_key_expr(pcb);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	/* check the object specified in the key expression */
	if (pcb->obj && pcb->altobj) {
	    if (pcb->altobj->objtype != OBJ_TYP_LEAF) {
		res = ERR_NCX_WRONG_TYPE;
		log_error("\nError: path target found is %s '%s'",
			  obj_get_typestr(pcb->altobj),
			  obj_get_name(pcb->altobj));
		ncx_mod_exp_err(pcb->tkc, 
				pcb->objmod, 
				res, "leaf");
		done = TRUE;
		continue;
	    } else {
		/* check the predicate for errors */
		if (pcb->altobj == pcb->obj) {
		    res = ERR_NCX_DEF_LOOP;
		    log_error("\nError: path target '%s' is set to "
			      "the target object",
			      obj_get_name(pcb->altobj));
		    ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
		    done = TRUE;
		    continue;
		} else if (pcb->altobj == pcb->varobj) {
		    res = ERR_NCX_DEF_LOOP;
		    log_error("\nError: path target '%s' is set to "
			      "the key leaf object",
			      obj_get_name(pcb->altobj));
		    ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
		    done = TRUE;
		    continue;
		}
	    }
	}

	/* get the right bracket ']' */
	res = parse_token(pcb, TK_TT_RBRACK);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	pcb->curmode = XP_CM_TARGET;

	/* check the next token;
	 * it may be the start of another path-predicate '['
	 */
	nexttyp = tk_next_typ(pcb->tkc);
	if (nexttyp != TK_TT_LBRACK) {
	    done = TRUE;
	} /* else keep going to the next path-predicate */
    }

    if (pcb->obj) {
	if (loopcount != keytotal) {
	    if (keycount < keytotal) {
		res = ERR_NCX_MISSING_INDEX;
		log_error("\nError: missing key components in"
			  " XPath expression for list '%s'",
			  obj_get_name(pcb->targobj));
	    } else {
		res = ERR_NCX_EXTRA_VAL_INST;
		log_error("\nError: extra key components in"
			  " XPath expression for list '%s'",
			  obj_get_name(pcb->targobj));
	    }
	    ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
	}
    }

    return res;

}  /* parse_path_predicate */


/********************************************************************
* FUNCTION parse_absolute_path
* 
* Parse the keyref path-arg string
* It has already been tokenized
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* absolute-path          = 1*("/" (node-identifier *path-predicate))
*
* path-predicate         = "[" *WSP path-equality-expr *WSP "]"
*
* path-equality-expr     = node-identifier *WSP "=" *WSP path-key-expr
*
* path-key-expr          = current-function-invocation "/"
*                          rel-path-keyexpr
*
* node-identifier        = [prefix ":"] identifier
*
* current-function-invocation = 'current()'
*
* INPUTS:
*    pcb == parser control block in progress
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    parse_absolute_path (xpath_pcb_t *pcb)
{
    tk_type_t    nexttyp;
    status_t     res;
    boolean      done;

    res = NO_ERR;
    done = FALSE;

    /* make one loop for each step */
    while (!done) {
	/* get  the first token in the step, '/' */
	res = parse_token(pcb, TK_TT_FSLASH);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	/* get the node-identifier next */
	res = parse_node_identifier(pcb);
	if (res != NO_ERR) {
	    done = TRUE;
	    continue;
	}

	/* check the next token;
	 * it may be the start of a path-predicate '['
	 * or the start of another step '/'
	 */
	nexttyp = tk_next_typ(pcb->tkc);
	if (nexttyp == TK_TT_LBRACK) {
	    res = parse_path_predicate(pcb);
	    if (res != NO_ERR) {
		done = TRUE;
		continue;
	    }

	    nexttyp = tk_next_typ(pcb->tkc);
	    if (nexttyp != TK_TT_FSLASH) {
		done = TRUE;
	    }
	} else if (nexttyp != TK_TT_FSLASH) {
	    done = TRUE;
	} /* else keep going to the next step */
    }

    /* check that the string ended properly */
    if (res == NO_ERR) {
	nexttyp = tk_next_typ(pcb->tkc);
	if (nexttyp != TK_TT_NONE) {
	    res = ERR_NCX_INVALID_TOKEN;
	    log_error("\nError: wrong token at end of absolute-path '%s'",
		      tk_get_token_name(nexttyp));
	    ncx_print_errormsg(pcb->tkc, pcb->mod, res);
	}
    }

    return res;

}  /* parse_absolute_path */


/********************************************************************
* FUNCTION parse_relative_path
* 
* Parse the keyref relative-path string
* It has already been tokenized
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* relative-path          = descendant-path /
*                          (".." "/"
*                          *relative-path)
*
* descendant-path        = node-identifier *path-predicate
*                          absolute-path
*
* Real implementation:
*
* relative-path          = *(".." "/") descendant-path
*
* descendant-path        = node-identifier *path-predicate
*                          [absolute-path]
*
* INPUTS:
*    pcb == parser control block in progress
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    parse_relative_path (xpath_pcb_t *pcb)
{
    tk_type_t    nexttyp;
    status_t     res;

    res = NO_ERR;

    /* check the next token;
     * it may be the start of a '../' pair or a
     * descendant-path (node-identifier)
     */
    nexttyp = tk_next_typ(pcb->tkc);
    while (nexttyp == TK_TT_RANGESEP && res == NO_ERR) {
	res = parse_token(pcb, TK_TT_RANGESEP);
	if (res == NO_ERR) {
	    res = parse_token(pcb, TK_TT_FSLASH);
	    if (res == NO_ERR) {
		if (pcb->obj) {
		    res = move_up_obj(pcb);
		}

		/* check the next token;
		 * it may be the start of another ../ pair
		 * or a node identifier
		 */
		nexttyp = tk_next_typ(pcb->tkc);
	    }
	}
    }

    if (res == NO_ERR) {
	/* expect a node identifier first */
	res = parse_node_identifier(pcb);
	if (res == NO_ERR) {
	    /* check the next token;
	     * it may be the start of a path-predicate '['
	     * or the start of another step '/'
	     */
	    nexttyp = tk_next_typ(pcb->tkc);
	    if (nexttyp == TK_TT_LBRACK) {
		res = parse_path_predicate(pcb);
	    }

	    if (res == NO_ERR) {
		/* check the next token;
		 * it may be the start of another step '/'
		 */
		nexttyp = tk_next_typ(pcb->tkc);
		if (nexttyp == TK_TT_FSLASH) {
		    res = parse_absolute_path(pcb);
		}
	    }
	}
    }
	    
    return res;

}  /* parse_relative_path */


/********************************************************************
* FUNCTION parse_path_arg
* 
* Parse the keyref path-arg string
* It has already been tokenized
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* path-arg-str           = < a string which matches the rule
*                            path-arg >
*
* path-arg               = absolute-path / relative-path
*
* INPUTS:
*    pcb == parser control block in progress
*
* RETURNS:
*   status
*********************************************************************/
static status_t
    parse_path_arg (xpath_pcb_t *pcb)
{
    tk_type_t  nexttyp;

    nexttyp = tk_next_typ(pcb->tkc);
    if (nexttyp == TK_TT_FSLASH) {
	pcb->abspath = TRUE;
	return parse_absolute_path(pcb);
    } else {
	return parse_relative_path(pcb);
    }

}  /* parse_path_arg */


/************    E X T E R N A L   F U N C T I O N S    ************/


/*********    S C H E M A  N O D E  I D    S U P P O R T  ***********/


/********************************************************************
* FUNCTION xpath_find_schema_target
* 
* Follow the absolute-path or descendant-node path expression
* and return the obj_template_t that it indicates, and the
* que that the object is in
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    tkc == token chain in progress  (may be NULL: errmsg only)
*    mod == module in progress
*    obj == augment object initiating search, NULL to start at top
*    datadefQ == Q of obj_template_t containing 'obj'
*    target == Xpath expression string to evaluate
*    targobj == address of return object  (may be NULL)
*    targQ == address of return target queue (may be NULL)
*
* OUTPUTS:
*   if non-NULL inputs:
*      *targobj == target object
*      *targQ == datadefQ Q header which contains targobj
*
* RETURNS:
*   status
*********************************************************************/
status_t
    xpath_find_schema_target (tk_chain_t *tkc,
			      ncx_module_t *mod,
			      obj_template_t *obj,
			      dlq_hdr_t  *datadefQ,
			      const xmlChar *target,
			      obj_template_t **targobj,
			      dlq_hdr_t **targQ)
{
    return xpath_find_schema_target_err(tkc, mod, obj, datadefQ, 
					target, targobj, targQ, 
					NULL);

}  /* xpath_find_schema_target */


/********************************************************************
* FUNCTION xpath_find_schema_target_err
* 
* Same as xpath_find_schema_target except a token struct
* is provided to use for the error token, instead of 'obj'
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    tkc == token chain in progress (may be NULL: errmsg only)
*    mod == module in progress
*    obj == augment object initiating search, NULL to start at top
*    datadefQ == Q of obj_template_t containing 'obj'
*    target == Xpath expression string to evaluate
*    targobj == address of return object  (may be NULL)
*    targQ == address of return target queue (may be NULL)
*    errtk == error token to use if any messages generated
*
* OUTPUTS:
*   if non-NULL inputs:
*      *targobj == target object
*      *targQ == datadefQ header for targobj
*
* RETURNS:
*   status
*********************************************************************/
status_t
    xpath_find_schema_target_err (tk_chain_t *tkc,
				  ncx_module_t *mod,
				  obj_template_t *obj,
				  dlq_hdr_t  *datadefQ,
				  const xmlChar *target,
				  obj_template_t **targobj,
				  dlq_hdr_t **targQ,
				  tk_token_t *errtk)
{
    status_t    res;

#ifdef DEBUG
    if (!mod || !datadefQ || !target) {
	return SET_ERROR(ERR_INTERNAL_PTR);
    }
#endif

    if (*target == '/') {
	/* check error: if nested object is using an abs. Xpath */
	if (obj && obj->parent) {
	    log_error("\nError: Absolute Xpath expression not "
		      "allowed here (%s)", target);
	    res = ERR_NCX_INVALID_VALUE;
	    do_errmsg(tkc, mod, errtk ? errtk : obj->tk, res);
	    return res;
	}
    }

    res = find_schema_node(tkc, mod, obj, datadefQ,
			   target, targobj, targQ, 
			   errtk ? errtk : (obj ? obj->tk : NULL));
    return res;

}  /* xpath_find_schema_target_err */


/********************************************************************
* FUNCTION xpath_find_schema_int
* 
* Follow the absolute-path expression
* and return the obj_template_t that it indicates
*
* Internal access version
* Error messages are not printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    target == absolute Xpath expression string to evaluate
*    targobj == address of return object  (may be NULL)
*
* OUTPUTS:
*   if non-NULL inputs:
*      *targobj == target object
*
* RETURNS:
*   status
*********************************************************************/
status_t
    xpath_find_schema_target_int (const xmlChar *target,
				  obj_template_t **targobj)
{
#ifdef DEBUG
    if (!target) {
	return SET_ERROR(ERR_INTERNAL_PTR);
    }
#endif

    return find_schema_node_int(target, targobj);

}  /* xpath_find_schema_target_int */


/********************************************************************
* FUNCTION xpath_find_val_target
* 
* Follow the absolute-path Xpath expression as used
* internally to identify a config DB node
* and return the obj_template_t that it indicates
*
* Expression must be the node-path from root for
* the desired node.
*
* Error messages are logged by this function
*
* INPUTS:
*    cfg == configuration to search
*    mod == module to use for the default context
*           and prefixes will be relative to this module's
*           import statements.
*        == NULL and the default registered prefixes
*           will be used
*    target == Xpath expression string to evaluate
*    targval == address of return value  (may be NULL)
*
* OUTPUTS:
*   if non-NULL inputs and value node found:
*      *targval == target value node
*   If non-NULL targval and error exit:
*      *targval == last good node visited in expression (if any)
*
* RETURNS:
*   status
*********************************************************************/
status_t
    xpath_find_val_target (val_value_t *startval,
			   ncx_module_t *mod,
			   const xmlChar *target,
			   val_value_t **targval)
{

#ifdef DEBUG
    if (!startval || !target) {
	return SET_ERROR(ERR_INTERNAL_PTR);
    }
#endif

    return find_val_node(startval, mod, target, targval);

}  /* xpath_find_val_target */


/*******    X P A T H   and   K E Y R E F    S U P P O R T   *******/


/********************************************************************
* FUNCTION xpath_new_pcb
* 
* Create and initialize an XPath parser control block
*
* INPUTS:
*   XPath expression string to save (a copy will be made)
*
* RETURNS:
*   pointer to malloced struct, NULL if malloc error
*********************************************************************/
xpath_pcb_t *
    xpath_new_pcb (const xmlChar *xpathstr)
{
    xpath_pcb_t *pcb;

#ifdef DEBUG
    if (!xpathstr) {
	SET_ERROR(ERR_INTERNAL_PTR);
	return NULL;
    }
#endif

    pcb = m__getObj(xpath_pcb_t);
    if (!pcb) {
	return NULL;
    }

    memset(pcb, 0x0, sizeof(xpath_pcb_t));

    pcb->exprstr = xml_strdup(xpathstr);
    if (!pcb->exprstr) {
	m__free(pcb);
	return NULL;
    }

    dlq_createSQue(&pcb->varbindQ);
    dlq_createSQue(&pcb->result.nodeQ);

    return pcb;

}  /* xpath_new_pcb */


/********************************************************************
* FUNCTION xpath_free_pcb
* 
* Free a malloced XPath parser control block
*
* INPUTS:
*   pcb == pointer to parser control block to free
*********************************************************************/
void
    xpath_free_pcb (xpath_pcb_t *pcb)
{
    val_value_t  *val;

    while (!dlq_empty(&pcb->varbindQ)) {
	val = (val_value_t *)dlq_deque(&pcb->varbindQ);
	val_free_value(val);
    }

    if (pcb->tkc) {
	tk_free_chain(pcb->tkc);
    }

    if (pcb->exprstr) {
	m__free(pcb->exprstr);
    }

    while (!dlq_empty(&pcb->result.nodeQ)) {
	val = (val_value_t *)dlq_deque(&pcb->result.nodeQ);
	val_free_value(val);
    }

    m__free(pcb);

}  /* xpath_free_pcb */


/********************************************************************
* FUNCTION xpath_parse_keyref_path
* 
* Parse the keyref path as a keyref path
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
*
* INPUTS:
*    tkc == parent token chain
*    mod == module in progress
*    pcb == initialized xpath parser control block
*           for the keyref path; use xpath_new_pcb
*           to initialize before calling this fn
*
* OUTPUTS:
*   pcb->tkc is filled and then validated
*
* RETURNS:
*   status
*********************************************************************/
status_t
    xpath_parse_keyref_path (tk_chain_t *tkc,
			     ncx_module_t *mod,
			     xpath_pcb_t *pcb)
{
    status_t       res;

#ifdef DEBUG
    if (!tkc || !mod || !pcb) {
	return SET_ERROR(ERR_INTERNAL_PTR);
    }
#endif

    /* before all objects are known, only simple validation
     * is done, and the token chain is saved for reuse
     * each time the expression is evaluated
     */
    pcb->tkc = tk_tokenize_xpath_string(mod, pcb->exprstr, 
					TK_CUR_LNUM(tkc),
					TK_CUR_LPOS(tkc),
					&res);
    if (!pcb->tkc || res != NO_ERR) {
	log_error("\nError: Invalid path string '%s'",
		  pcb->exprstr);
	ncx_print_errormsg(tkc, mod, res);
	return res;
    }

    /* the module that contains the keyref is the one
     * that will always be used to resolve prefixes
     * within the XPath expression
     */
    pcb->mod = mod;
    pcb->source = XP_SRC_KEYREF;

    /* since the pcb->obj is not set, this validation
     * phase will skip identifier tests, predicate tests
     * and completeness tests
     */
    pcb->parseres = parse_path_arg(pcb);

    /* the expression will not be processed further if the
     * parseres is other than NO_ERR
     */
    return pcb->parseres;

}  /* xpath_parse_keyref_path */


/********************************************************************
* FUNCTION xpath_validate_keyref_path
* 
* Validate the previously parsed keyref path
*   - QNames are valid
*   - object structure referenced is valid
*   - objects are all 'config true'
*   - target object is a leaf
*   - keyref represents a single instance
*
* Called after all 'uses' and 'augment' expansion
* so validation against cooked object tree can be done
*
* Error messages are printed by this function!!
* Do not duplicate error messages upon error return
*
* INPUTS:
*    mod == module containing the 'obj' (in progress)
*    obj == object using the keyref data type
*    pcb == the keyref parser control block from the typdef
*
* RETURNS:
*   status
*********************************************************************/
status_t
    xpath_validate_keyref_path (ncx_module_t *mod,
				const obj_template_t *obj,
				xpath_pcb_t *pcb)
{
    status_t  res;

#ifdef DEBUG
    if (!mod || !obj || !pcb) {
	return SET_ERROR(ERR_INTERNAL_PTR);
    }
    if (!pcb->tkc) {
	return SET_ERROR(ERR_INTERNAL_VAL);
    }
#endif

    if (pcb->parseres != NO_ERR) {
	/* errors already reported, skip this one */
	return NO_ERR;
    }

    pcb->docroot = ncx_get_gen_root();
    if (!pcb->docroot) {
	return SET_ERROR(ERR_INTERNAL_VAL);
    }

    tk_reset_chain(pcb->tkc);
    pcb->objmod = mod;
    pcb->obj = obj;
    pcb->source = XP_SRC_KEYREF;
    pcb->targobj = NULL;
    pcb->altobj = NULL;
    pcb->varobj = NULL;
    pcb->curmode = XP_CM_TARGET;

    /* validate the XPath expression against the 
     * full cooked object tree
     */
    pcb->validateres = parse_path_arg(pcb);

    /* check keyref is config but target is not */
    if (pcb->validateres == NO_ERR && pcb->targobj) {
	if (obj_get_config_flag(obj) &&
	    !obj_get_config_flag(pcb->targobj)) {
	    res = ERR_NCX_NOT_CONFIG;
	    log_error("\nError: XPath target '%s' for keyref '%s' must be "
		      "a config object",
		      obj_get_name(pcb->targobj),
		      obj_get_name(obj));
	    ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
	    pcb->validateres = res;
	}

	switch (pcb->source) {
	case XP_SRC_KEYREF:
	    if (!obj_is_key(pcb->targobj)) {
		res = ERR_NCX_TYPE_NOT_INDEX;
		log_error("\nError: path target '%s' is "
			  "not a key leaf",
			  obj_get_name(pcb->targobj));
		ncx_mod_exp_err(pcb->tkc, 
				pcb->objmod, 
				res, "key leaf");
		pcb->validateres = res;
	    }
	    break;
	case XP_SRC_LEAFREF:
	    break;
	default:
	    ;
	}

	if (pcb->targobj == pcb->obj) {
	    res = ERR_NCX_DEF_LOOP;
	    log_error("\nError: path target '%s' is set to "
		      "the target object",
		      obj_get_name(pcb->targobj));
	    ncx_print_errormsg(pcb->tkc, pcb->objmod, res);
	}
    }

    return pcb->validateres;

}  /* xpath_validate_keyref_path */


/********************************************************************
* FUNCTION xpath_get_keyref_value
* 
* Get a pointer to the keyref target value node
*
* INPUTS:
*    mod == module in progress
*    obj == object initiating search, which contains the keyref type
*    pcb == XPath parser control block to use
*    targval == address of return target value (may be NULL)
*
* OUTPUTS:
*   if non-NULL:
*      *targval == pointer to return value node target
*
* RETURNS:
*   status
*********************************************************************/
status_t
    xpath_get_keyref_value (ncx_module_t *mod,
			    obj_template_t *obj,
			    xpath_pcb_t *pcb,
			    val_value_t **targval)
{

#ifdef DEBUG
    if (!mod || !obj || !pcb) {
	return SET_ERROR(ERR_INTERNAL_PTR);
    }
#endif

    /*****/

    return NO_ERR;

}  /* xpath_get_keyref_value */


/* END xpath.c */
