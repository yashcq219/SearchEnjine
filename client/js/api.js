/**
 * REST API Client for SearchEngine C++ Backend
 */
const POSSIBLE_PORTS = ['18080', '8080'];
let activeBaseUrl = `http://localhost:${POSSIBLE_PORTS[0]}`;

export async function checkHealth() {
    for (const port of POSSIBLE_PORTS) {
        const candidateUrl = `http://localhost:${port}`;
        try {
            const response = await fetch(`${candidateUrl}/health`, {
                method: 'GET',
                headers: { 'Accept': 'application/json' }
            });
            if (response.ok) {
                activeBaseUrl = candidateUrl;
                const data = await response.json();
                return { online: true, data, port };
            }
        } catch (_) {
            // Try next port candidate
        }
    }
    return { online: false, error: 'Server unreachable on port 18080 or 8080' };
}

export async function search(query) {
    const startTime = performance.now();
    try {
        const response = await fetch(`${activeBaseUrl}/search?q=${encodeURIComponent(query)}`, {
            method: 'GET',
            headers: { 'Accept': 'application/json' }
        });

        const endTime = performance.now();
        const durationMs = Math.round(endTime - startTime);

        if (!response.ok) {
            const errorData = await response.json().catch(() => ({}));
            return {
                success: false,
                status: response.status,
                error: errorData.error || `HTTP ${response.status} Error`,
                durationMs
            };
        }

        const data = await response.json();
        return {
            success: true,
            results: data.results || [],
            durationMs
        };
    } catch (err) {
        const endTime = performance.now();
        return {
            success: false,
            error: 'Failed to connect to SearchEngine server. Please verify the C++ server executable is running.',
            durationMs: Math.round(endTime - startTime)
        };
    }
}
