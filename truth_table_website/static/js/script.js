let sessionId = null;

/**
 * Generate the session id
 */
function generateSessionId() {
    return 'session_' + Math.random().toString(36).substr(2, 9);
}

/**
 * Sends a request to clear the cache using the sessionId.
 */
function sendCacheClearRequest() {
    fetch(`/clear_cache?session_id=${encodeURIComponent(sessionId)}`, {
        method: 'POST'
    }).then(response => {
        if (!response.ok) {
            console.error("Failed to clear cache.");
        } else {
            console.log("Cache cleared successfully.");
        }
    }).catch(error => console.error("Error clearing cache:", error));
}

/**
 * Changes the mode when each respective button is clicked.
 */
function handleButtonClick(mode) {
    document.getElementById('mode').value = mode;
}

/**
 * Clear the cache every time the page reloads - so every time the user generates a new table or when they close the webpage.
 */
window.addEventListener('beforeunload', function (event) {
    if (sessionId) {
        sendCacheClearRequest();
    }
});

/**
 * Function to get and  display the rows from the server.
 */
document.addEventListener("DOMContentLoaded", () => {
    const tableContent = document.getElementById("table-content");
    const preElement = document.querySelector(".output-panel pre");
    sessionId = generateSessionId();



    // Variables for pagination
    let start = 0;
    const rowsPerPage = 100;
    let end = rowsPerPage;
    let isFetching = false;

    async function fetchRows(start, end) {
        isFetching = true;

        try {
            const expression = document.getElementById('input_text').value;
            const mode = document.getElementById('mode').value;
            const response = await fetch(
                `/generate?expression=${encodeURIComponent(expression)}&start=${start}&end=${end}&mode=${encodeURIComponent(mode)}&session_id=${encodeURIComponent(sessionId)}`
            );

            if (!response.ok) throw new Error('Failed to fetch rows');
            const data = await response.json();

            if (data.length === 0) return;

            data.forEach((row) => {
                const rowElement = document.createElement("div");
                rowElement.textContent = row;
                tableContent.appendChild(rowElement);
            });
        } catch (error) {
            console.error("Error fetching rows:", error);
        } finally {
            isFetching = false;
        }
    }

    // Load initial rows
    fetchRows(start, end);

    // Scroll event listener for scrolling
    preElement.addEventListener('scroll', () => {
        const scrollTop = preElement.scrollTop;
        const scrollHeight = preElement.scrollHeight;
        const clientHeight = preElement.clientHeight;

        if (!isFetching && Math.ceil(scrollTop + clientHeight) >= scrollHeight - 10) {
            start = end;
            end += rowsPerPage;
            fetchRows(start, end);
        }
    });
});
