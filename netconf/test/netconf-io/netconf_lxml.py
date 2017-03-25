import netconf
import lxml
from lxml import etree

def strip_namespaces(tree):
	xslt='''<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="no"/>

    <xsl:template match="/|comment()|processing-instruction()">
        <xsl:copy>
          <xsl:apply-templates/>
        </xsl:copy>
    </xsl:template>

    <xsl:template match="*">
        <xsl:element name="{local-name()}">
          <xsl:apply-templates select="@*|node()"/>
        </xsl:element>
    </xsl:template>

    <xsl:template match="@*">
        <xsl:attribute name="{local-name()}">
          <xsl:value-of select="."/>
        </xsl:attribute>
    </xsl:template>
    </xsl:stylesheet>
    '''
	xslt_doc = lxml.etree.fromstring(xslt)
	transform = lxml.etree.XSLT(xslt_doc)
	tree = transform(tree)
	return tree

class netconf_lxml():
	def __init__(self, netconf):
		self.netconf=netconf
		self.strip_namespaces=True

	def receive(self):
		(ret,reply_xml)=self.netconf.receive()
		if(ret!=0):
			return None
		reply_lxml = lxml.etree.fromstring(reply_xml)
		if(self.strip_namespaces):
			reply_lxml_striped=strip_namespaces(reply_lxml)
 		return reply_lxml_striped

	def rpc(self, xml, message_id=1):
		ret=self.netconf.send('''<rpc xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="'''+str(message_id)+'''">'''+xml+"</rpc>")
		if(ret!=0):
			return None
		reply_lxml=self.receive()
		return reply_lxml

	def send(self, xml):
		return self.netconf.send(xml)
