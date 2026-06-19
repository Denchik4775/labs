import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import glob
import os

st.set_page_config(page_title="Аналіз NOAA VHI", layout="wide")

@st.cache_data
def load_and_clean_data(data_dir="vhi_data"):
    current_dir = os.path.dirname(os.path.abspath(__file__))
    target_path = os.path.join(current_dir, data_dir)
    files = glob.glob(os.path.join(target_path, "*.csv"))

    data = []
    for file in files:
        try:
            prov_id = int(os.path.basename(file).split('_')[2])
            with open(file, 'r') as f:
                for line in f:
                    line = line.replace('<tt><pre>', '').replace('</pre></tt>', '').strip()
                    if not line or 'year' in line.lower(): continue
                    parts = [x.strip() for x in line.split(',')]
                    if len(parts) >= 7:
                        data.append({'NOAA_ID': prov_id, 'Year': int(parts[0]), 'Week': int(parts[1]), 
                                     'VCI': float(parts[4]), 'TCI': float(parts[5]), 'VHI': float(parts[6])})
        except: continue

    df = pd.DataFrame(data)
    ua_names = {1: 'Вінницька', 2: 'Волинська', 3: 'Дніпропетровська', 4: 'Донецька', 5: 'Житомирська',
                6: 'Закарпатська', 7: 'Запорізька', 8: 'Івано-Франківська', 9: 'Київська', 10: 'Кіровоградська',
                11: 'Луганська', 12: 'Львівська', 13: 'Миколаївська', 14: 'Одеська', 15: 'Полтавська',
                16: 'Рівненська', 17: 'Сумська', 18: 'Тернопільська', 19: 'Харківська', 20: 'Херсонська',
                21: 'Хмельницька', 22: 'Черкаська', 23: 'Чернівецька', 24: 'Чернігівська', 25: 'АР Крим',
                26: 'м. Київ', 27: 'м. Севастополь'}
    if not df.empty:
        df['Province_Name'] = df['NOAA_ID'].map(ua_names)
    return df

df = load_and_clean_data()
if df.empty:
    st.error("Дані не знайдено! Перевірте папку 'vhi_data'.")
    st.stop()

st.sidebar.header("Параметри аналізу")
metric = st.sidebar.selectbox("Індекс", ['VCI', 'TCI', 'VHI'])
selected_province = st.sidebar.selectbox("Область", sorted(df['Province_Name'].unique()))
years = st.sidebar.slider("Роки", int(df['Year'].min()), int(df['Year'].max()), (1981, 2024))
weeks = st.sidebar.slider("Тижні", 1, 52, (1, 52))

if st.sidebar.button("Скинути фільтри"): st.rerun()

filtered_df = df[(df['Province_Name'] == selected_province) & 
                 (df['Year'].between(*years)) & (df['Week'].between(*weeks))]

tab1, tab2, tab3 = st.tabs(["Таблиця", "Часовий ряд", "Порівняння"])
with tab1:
    st.dataframe(filtered_df)
with tab2:
    st.line_chart(filtered_df.set_index('Week')[metric])
with tab3:
    fig, ax = plt.subplots(figsize=(10, 5))
    mean_vals = df[df['Year'].between(*years)].groupby('Province_Name')[metric].mean().reset_index()
    colors = ['red' if n == selected_province else 'lightgray' for n in mean_vals['Province_Name']]
    sns.barplot(data=mean_vals, x='Province_Name', y=metric, palette=colors, ax=ax)
    plt.xticks(rotation=90)
    st.pyplot(fig)
