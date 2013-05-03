<?xml version="1.0"?>

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template match="/">
    <table>
      <tr class="header">
        <th>Channel</th>
        <th>Upload Speed (bytes/s)</th>
        <th>Connected Peers</th>
        <th>Uploading Peers</th>
        <th>First Cached Block</th>
        <th>Last Cached Block</th>
      </tr>
      <xsl:for-each select="live_status/channel_status/channel">
        <tr>
          <td>
            <xsl:value-of select="@id"/>
          </td>
          <td>
            <xsl:value-of select="status/@upload_speed"/>
          </td>
          <td>
            <xsl:value-of select="status/@user_count"/>
          </td>
          <td>
            <xsl:value-of select="status/@uploading_user_count"/>
          </td>
          <td>
            <xsl:value-of select="status/@begin_time"/>
          </td>
          <td>
            <xsl:value-of select="status/@end_time"/>
          </td>
        </tr>
      </xsl:for-each>
    </table>
  </xsl:template>

</xsl:stylesheet>
