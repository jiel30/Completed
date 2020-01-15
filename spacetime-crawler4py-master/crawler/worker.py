from threading import Thread
import re
from utils.download import download
from utils import get_logger
from scraper import scraper,is_valid
import time

url_path = '/Users/lijie/Desktop/url_list.txt'
def process_html(html):
    '''process html to tokenize to get a list of words
        '''
    table = str.maketrans(".,?!'\";:-_(){}[]\|`~#$%^&*<:>/+=","                                ")
    reg = re.compile('<[^>]*>')
    html = reg.sub('',html.decode().replace('\n','').replace(' ',''))
    text = html.translate(table)
    words = text.split()
    return words
def delete_fragment(url:str):
    a = 0
    for char in url:
        if char == '#':
            return url[0:a]
        a+=1
    return url
def update_url(url,code,html):
    file = open(url_path,'a')
    try:
        length = len(process_html(html))
    except:
        length = 0
    file.write(delete_fragment(url) + ' ' + code + ' ' + str(length) + '\n')
    file.close()
class Worker(Thread):
    def __init__(self, worker_id, config, frontier):
        self.logger = get_logger(f"Worker-{worker_id}", "Worker")
        self.config = config
        self.frontier = frontier
        super().__init__(daemon=True)
        
    def run(self):
        while True:
            tbd_url = self.frontier.get_tbd_url()
            if not tbd_url:
                self.logger.info("Frontier is empty. Stopping Crawler.")
                break
            resp = download(tbd_url, self.config, self.logger)
            if resp == None:
                self.frontier.mark_url_complete(tbd_url)
                time.sleep(self.config.time_delay)
                continue
            try:
                update_url(tbd_url,str(resp.status),resp.raw_response.content)   ##store the url
            except:
                continue
            self.logger.info(
                f"Downloaded {tbd_url}, status <{resp.status}>, "
                f"using cache {self.config.cache_server}.")
            scraped_urls = scraper(tbd_url, resp)
            for scraped_url in scraped_urls:
                self.frontier.add_url(scraped_url)
            self.frontier.mark_url_complete(tbd_url)
            time.sleep(self.config.time_delay)
