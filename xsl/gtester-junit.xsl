<?xml version="1.0" encoding="utf-8"?>
<!--
 Copyright (C) 2011-2014 Gary Kramlich <grim@reaperworld.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, see <http://www.gnu.org/licenses/>.
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" cdata-section-elements="system-out system-err failure" indent="yes"/>

	<!-- basename template, keeps removing text before '/' until there isn't any -->
	<xsl:template name="basename">
		<xsl:param name="path"/>

		<xsl:choose>
			<xsl:when test="contains($path, '/')">
				<xsl:call-template name="basename">
					<xsl:with-param name="path" select="substring-after($path, '/')"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$path"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- match the gtester root tag -->
	<xsl:template match="/gtester">
		<xsl:element name="testsuit">
			<!-- add the number of tests as the test attribute -->
			<xsl:attribute name="tests">
				<xsl:value-of select="count(//testcase)"/>
			</xsl:attribute>

			<!-- sum up all durations and add it as the time attribute -->
			<xsl:attribute name="time">
				<xsl:value-of select="sum(//testbinary/duration)"/>
			</xsl:attribute>

			<!-- add the number of failures as the failures attribute -->
			<xsl:attribute name="failures">
				<xsl:value-of select="count(//testcase/status[@result='failed'])"/>
			</xsl:attribute>

			<xsl:apply-templates/>

			<!-- now create the system-out and system-err elements -->
			<xsl:element name="system-out">
				<xsl:apply-templates select="//message"/>
			</xsl:element>

			<xsl:element name="system-err">
				<xsl:apply-templates select="//error"/>
			</xsl:element>
		</xsl:element>
	</xsl:template>

	<!-- match the testcases -->
	<xsl:template match="/gtester/testbinary/testcase">
		<xsl:element name="testcase">
			<xsl:attribute name="time">
				<xsl:value-of select="./duration/text()"/>
			</xsl:attribute>
			<xsl:attribute name="name">
				<xsl:variable name="exec">
					<xsl:call-template name="basename">
						<xsl:with-param name="path" select="../@path"/>
					</xsl:call-template>
				</xsl:variable>
				<xsl:value-of select="concat($exec, translate(@path, '/-', '.'))"/>
			</xsl:attribute>

			<xsl:if test="./status[@result='failed']">
				<xsl:element name="failure">
					<xsl:value-of select="error"/>
				</xsl:element>
			</xsl:if>
		</xsl:element>
	</xsl:template>

	<!-- ignore all other cdata -->
	<xsl:template match="text()"/>
</xsl:stylesheet>

