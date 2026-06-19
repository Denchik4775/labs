import os
import urllib.request
import datetime

def download_noaa_data(target_dir="vhi_data"):
    os.makedirs(target_dir, exist_ok=True)
    now = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    
    for prov_id in range(1, 28):
        url = f"https://www.star.nesdis.noaa.gov/smcd/emb/vci/VH/get_TS_admin.php?country=UKR&provinceID={prov_id}&year1=1981&year2=2024&type=Mean"
        filename = f"vhi_prov_{prov_id}_{now}.csv"
        filepath = os.path.join(target_dir, filename)
        
        try:
            req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
            with urllib.request.urlopen(req) as response:
                text = response.read().decode('utf-8')
            with open(filepath, 'w') as f:
                f.write(text)
            print(f"Завантажено: {filename}")
        except Exception as e:
            print(f"Помилка для {prov_id}: {e}")

if __name__ == "__main__":
    download_noaa_data()