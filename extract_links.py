#!/usr/bin/env python

import sys
import urllib2
import chardet
from HTMLParser import HTMLParser

class LinkExtractor(HTMLParser):
    def handle_starttag(self, tag, attrs):
        if tag == 'a':
            for name, value in attrs:
                if name == 'href':
                    print value

def read_url(name):
    parser = LinkExtractor()
    html = urllib2.urlopen(name).read()
    encoding = chardet.detect(html)['encoding']
    print "READY to parse!"
    if encoding != 'unicode':
        html = html.decode(encoding)
    parser.feed(html)

def main():
    read_url(sys.argv[1])

if __name__ == '__main__':
    main()

