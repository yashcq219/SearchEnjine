import { store } from './state.js';
import { initNavbar } from './components/Navbar.js';
import { initSearchBar } from './components/SearchBar.js';
import { initResultsList } from './components/ResultsList.js';
import { initStats } from './components/Stats.js';

document.addEventListener('DOMContentLoaded', () => {
    initNavbar(store);
    initSearchBar(store);
    initStats(store);
    initResultsList(store);
});
