export function initStats(store) {
    const statsBanner = document.getElementById('stats-banner');

    store.subscribe((state) => {
        if (!state.hasSearched || state.isLoading) {
            statsBanner.style.display = 'none';
            return;
        }

        statsBanner.style.display = 'flex';
        const count = state.results.length;
        const timeStr = (state.durationMs / 1000).toFixed(2);

        statsBanner.innerHTML = `
            <span class="stats-count">About ${count} result${count === 1 ? '' : 's'} for <strong>"${escapeHtml(state.query)}"</strong></span>
            <span class="stats-time">Search completed in ${timeStr}s (${state.durationMs} ms)</span>
        `;
    });
}

function escapeHtml(str) {
    return str.replace(/&/g, '&amp;')
              .replace(/</g, '&lt;')
              .replace(/>/g, '&gt;')
              .replace(/"/g, '&quot;');
}
