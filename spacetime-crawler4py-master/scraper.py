import re
from urllib.parse import urlparse
import requests
from bs4 import BeautifulSoup as bs
import pickle
from urllib.request import urlopen
from lxml import etree,html
import urllib
##added function
dict_path = '/Users/lijie/Desktop/word_count.txt'
longest_path = '/Users/lijie/Desktop/longest.txt'
url_path = '/Users/lijie/Desktop/url_list.txt'
error_url = '/Users/lijie/Desktop/error_url.txt'
all_urls ='/Users/lijie/Desktop/all_url.txt'

def avoid_calendar(url):
    a = url.split('/')
    if (re.match(r"\d{4}-\d{2}-\d{2}$", a[-1])) or (re.match(r"\d{4}-\d{2}$", a[-1])) :
        return False
    return True

def compare_url(checks):
    '''put in one list which contains 1 or 2 urls, compare urls to return string which the parts are same
        '''
    if len(checks) == 1:
        return checks[0]
    first = checks[0].split('/')
    second = checks[1].split('/')
    final_str = ''
    count = 0
    for a in first:
        if count < len(second):
            if a == second[count]:
                final_str += a
                final_str += '/'
            else:
                temp_1 = a
                temp_2 = second[count]
                break
        count +=1
    index = 0
    last_part = ''
    try:
        for element in temp_1:
            if element == temp_2[index]:
                last_part+=element
            else:
                break
            index +=1
        if len(last_part) >= 3:  ##avoid we filter url like www.ics.uci.edu/p
            final_str += last_part
        return final_str
    except:
        return final_str
    return final_str
def process_error(checks:list):
    start = checks[0].split()[0]
    for element in checks:
        e = element.split()
        if int(e[1]) in (200,300,301,302,303,304,305,306,307):
            return None
        else:
            start = compare_url([start, e[0]])
    if start == 'https://www.ics.uci.edu':
        return None
    return start
def process_low(check:list):
    '''this function can check whether we goes through a series of similar url which contain
        low information, it can check their pattern and return it
        it can also filter many websites that only contain file to download
    '''
    start = check[0].split()[0]
    for element in check:
        e = element.split()
        if int(e[2]) >= 25:
            return None
        else:
            start = compare_url([start, e[0]])
    if start == 'https://www.ics.uci.edu':
        return None
    return start
def determine_error():
    '''check local file which contains all urls and find
        if we get continous error from one pattern of urls
        get the similar pattern and store it to local file, so we can filte them
        '''
    file = open('/Users/lijie/Desktop/url_list.txt')
    tt = file.readlines()
    text = []
    for t in tt:
        if(len(t) > 5):
            try:
                int(t[-2])
                text.append(t)
            except:
                continue
    file.close()
    check = text[len(text) - 4:]
    checks = []
    for element in check:
        a = urllib.parse.urlparse(element).netloc
        n= urllib.parse.urlparse(element).path
        checks.append(n)
    error_url = process_error(checks)
    low_url = process_low(checks)
    good_error = True
    good_low = True
    if error_url == None:
        good_error = False
    if low_url == None:
        good_low = False
    if good_error:
        b = urllib.parse.urlparse(error_url)
        if b.netloc != 'www.ics.uci.edu' and b.path != '/' and b.path != '/\n' and b.path != '':
            file = open('/Users/lijie/Desktop/error_url.txt','a')
            file.write(a + error_url + '\n')
            file.close()
    if good_low:
        l =urllib.parse.urlparse(low_url)
        if l.netloc != 'www.ics.uci.edu' and l.path != '/' and l.path != '/\n' and l.path != '':
            file = open('/Users/lijie/Desktop/error_url.txt','a')
            file.write(a + low_url + '\n')
            file.close()


def computeWordFrequencies(token:list):
    count = {}
    for w in token:   ##O(n)
        if len(w) == 1:
            continue
        word = w.lower()
        ##avoid caps
        if word not in count:
            count[word] = 1
        else:
            count[word] += 1
    return count
def combine_dict(a,b):
    '''combine two dict {str:int}, addup int
    '''
    for key,value in a.items():
        if key not in b:
            b[key] = value
        else:
            b[key] += value
    return b
def process_html(html):
    '''process html to tokenize to get a list of words
    '''
    table = str.maketrans(".,?!'\";:-_(){}[]\|`~#$%^&*<:>/+=","                                ")
    reg = re.compile('<[^>]*>')
    html = reg.sub('',html.decode().replace('\n','').replace(' ',''))
    text = html.translate(table)
    words = text.split()
    return words
def count_word(path,words):
    '''tokenize words in html and get a dict of word count
        take local file which contains other dict and combine them and save again
        '''
    temp_dict = computeWordFrequencies(words)
    try:
        file = open(path,'rb')
        d = pickle.load(file)
        file.close()
        final_dict = combine_dict(temp_dict,d)
        file = open(path,'wb')
        pickle.dump(final_dict,file)
        file.close()
    except:
        file.close()
        file = open(path,'wb')
        pickle.dump(temp_dict,file)
        file.close()

def update_longest(words,url):
    length = len(words)
    file = open(longest_path,'r')
    words = file.read().split()
    file.close()
    if length > int(words[0]):
        file = open(longest_path,'w')
        file.write(str(length) + ' ')
        file.write(url)
        file.close()
def delete_fragment(url:str):
    a = 0
    for char in url:
        if char == '#':
            return url[0:a]
        a+=1
    return url
def update_all_url(url):
    '''update all url to find subdomains
    '''
    if 'ics.uci.edu' not in url:
        return
    file = open(all_urls,'a')
    file.write(delete_fragment(url) + '\n')
    file.close()
def update_url(url,code,html):
    '''update urls that can pass the filter
    '''
    file = open(url_path,'a')
    length = len(process_html(html))
    urls = []
    for u in url:
        urls.append(delete_fragment(u))
    urls = set(urls)
    for u in urls:
        file.write(u + ' ' + code + ' ' + length + '\n')
    file.close()


def scraper(url, resp):
    links = extract_next_links(url, resp)
    try:
        determine_error()    ##update url that cause comes back 404 a lot
    except:
        final_list =[link for link in links if is_valid(link)]
        return final_list
    final_list =[link for link in links if is_valid(link)]
#    pickle.dump(final_list, output)
#    output.close()
    return final_list

def extract_next_links(url, resp):
    all_url = []
    if (resp.status not in (200,300,301,302,303,304,305,306,307)) or (resp.error != None):
        return all_url
    html = etree.HTML(resp.raw_response.content)
    try:
        words = process_html(resp.raw_response.content)
        count_word(dict_path,words)   ##update the words frequency
        update_longest(words,url)   ##updata the longest page
    except:
        return all_url
    try:
        links = html.xpath(u"//a")   ##deal with empty html
    except AttributeError:
        return all_url
    for link in links:
        first = link.get("href")
        abs = urllib.parse.urljoin(url,first)
        if abs[-1] != "/":   ##avoid add same urls which are only different in the / at the end
            abs += "/"
        all_url.append(abs)
    return all_url

def filter(url):
    '''filter some key word
    '''
    temp = ['https://www.ics.uci.edu/alumni','https://www.ics.uci.edu/community/alumni','http://www.ics.uci.edu/alumni']
    serious = ['https://www.ics.uci.edu/honors','http://www.ics.uci.edu/~seal/projects/deldroid/generic_system_eca_resources.img','http://www.ics.uci.edu/~majumder']
    ###serious is url that never response, to save time, we just refuse them
#    c = ['edu/alumni/','edu/community/news/spotlight','ps.gz','http://www.ics.uci.edu/~welling/classnotes/papers_class','http://www.ics.uci.edu/~welling','https://www.ics.uci.edu/~dechter','http://www.ics.uci.edu/~agelfand','http://www.ics.uci.edu/~dsm','http://www.ics.uci.edu/pubs','http://www.ics.uci.edu/~kay/courses/i41/quiz/old','https://www.ics.uci.edu/honors/computing','https://www.ics.uci.edu/~dsm/papers/','https://www.ics.uci.edu/community/news/notes_','https://www.ics.uci.edu/~dsm','http://www.ics.uci.edu/papers','http://www.ics.uci.edu/publications','http://www.ics.uci.edu/~dechter/r','https://www.ics.uci.edu/~kkask/publications/r','https://www.ics.uci.edu/honors','https://www.ics.uci.edu/~theory','https://www.ics.uci.edu/~dechter','https://www.ics.uci.edu/community/news/spotlight','https://www.ics.uci.edu/~dechter/courses/','http://www.ics.uci.edu/~agelfand/figs/largeFam3','http://www.ics.uci.edu/Arcadia/Teamware/docs/java','http://www.ics.uci.edu/%7Ethornton','http://www.ics.uci.edu/~eppstein','http://www.ics.uci.edu/~eppstein/ca','http://www.ics.uci.edu/~agelfand/figs','www.ics.uci.edu/~wscacchi/','www.ics.uci.edu/%7Ethornton/ProjectGuide/']+ serious
    file = open(error_url,'r')
    pattens = file.read().split('\n') + serious
#    pattens = c + serious + temp
#    pattens = [' https://www.ics.uci.edu/alumni'] + serious + temp
    file.close()
    for p in pattens:
        if p == '':
            continue
        elif p in url:
            return False
    return True
def avoid_repeat(url,number):
    '''detect if one word appear continously in one url too much time(>= 3 times)
        to filter them
    '''
    u = urllib.parse.urlparse(url).path
    aa = u.split('/')[1:]
    a = []
    index = 0;
    for element in aa:
        if index % number != 0:
            a[-1] = a[-1] + element
            index += 1
        else:
            a.append(element)
            index += 1
    if len(a) >=18:
        file = open(error_url,'a')
        file.write(url + '\n')
        file.close()   ##update error url
        return False
    final_dict = {}
    count = 0;
    temp = 0;
    for element in a:
        check_list = a[count:]
        check_count = 0;
        l = len(check_list)
        for check in check_list:
            if check_count < l - 2 and check == check_list[check_count + 1]:
                temp +=1
            else:
                break
            if temp >= 2:
                file = open(error_url,'a')
                file.write(url + '\n')
                file.close()  ##update error url
                return False
        count += 1
        temp = 0

    return True

def is_valid(url):
#    output = open('/Users/lijie/Desktop/url_list.txt','w')
#    output.write("start the is_valid\n")
#    output.close()
    try:
        update_all_url(url)   ##update all url to find subdomains
        if not filter(url):
            return False
        if not avoid_repeat(url,1):
            return False
        if not avoid_repeat(url,2):
            return False
        if not avoid_repeat(url,3):
            return False
        if not avoid_repeat(url,4):
            return False
        if not avoid_repeat(url,5):
            return False
        if not avoid_calendar(url):
            return False
        parsed = urlparse(url)
        if parsed.scheme not in set(["http", "https"]):
            return False
        if parsed.netloc != 'www.ics.uci.edu':
            return False
        return not re.match(
            r".*\.(css|js|bmp|gif|jpe?g|ico"
            + r"|png|tiff?|mid|mp2|mp3|mp4"
            + r"|wav|avi|mov|mpeg|ram|m4v|mkv|ogg|ogv|pdf"
            + r"|ps|eps|tex|ppt|pptx|doc|docx|xls|xlsx|names"
            + r"|data|dat|exe|bz2|tar|msi|bin|7z|psd|dmg|iso"
            + r"|epub|dll|cnf|tgz|sha1"
            + r"|thmx|mso|arff|rtf|jar|csv"
            + r"|rm|smil|wmv|swf|wma|zip|rar|gz|img)(/)?$", parsed.path.lower())

    except TypeError:
        print ("TypeError for ", parsed)
        raise

