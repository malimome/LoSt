import urllib

response = urllib.urlopen('http://api.hostip.info/get_html.php?ip=66.249.67.11&position=true').read()

print(response)
