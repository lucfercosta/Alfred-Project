**Project Alfred**

Project Alfred is a lightweight Linux application launcher inspired by tools such as Spotlight and Alfred. The project was created to improve productivity on Linux systems by providing a fast and efficient way to access applications, files, and directories through a keyboard-driven workflow.

Instead of manually navigating menus or file managers, users can instantly search for content using a lightweight in-memory search engine. The system performs prefix and partial string matching combined with adaptive ranking based on usage frequency. Over time, the launcher learns from user behavior and prioritizes frequently selected results.

The application is intentionally designed around simplicity, responsiveness, and low resource usage. The architecture avoids unnecessary complexity and focuses on predictable performance and maintainability.



**Architecture**

Project Alfred follows a three-tier architecture composed of the Presentation Layer, the Application Layer, and the Data Layer. This separation ensures modularity and maintains a clear distinction between user interaction, processing logic, and data management.

The Presentation Layer is responsible for all user interaction and visual rendering. It captures user input, displays search results, and forwards user actions to the application layer. The primary UI component is the ApplicationLauncherWidget, which acts as the central orchestrator of the launcher window. It manages the search box, result list, indexing coordination, and item execution flow. The layer also includes the SearchBoxResultListRenderer, which provides custom rendering behavior for the search results interface.

The Application Layer contains the core logic of the launcher. It is responsible for query processing, search execution, ranking, and launching selected items. The SearchEngine class handles query normalization, tokenization, matching, scoring, and ranking operations. The ApplicationLauncherService is responsible for translating selected search results into executable system actions such as launching applications or opening files and directories.

The Data Layer is responsible for collecting and storing all searchable entities. The FileSystemIndexer scans the operating system, indexes Linux applications, traverses directories, filters irrelevant content, and converts all discovered entries into lightweight searchable structures. The FileSystemIndexingWorker coordinates the indexing process and emits completion signals once indexing is finished.



**Runtime and Application Lifecycle**

Project Alfred uses an event-driven architecture where execution is determined by system events, user interaction, and inter-process communication.

The application entry point initializes the Qt event loop, enforces single-instance execution, and manages IPC communication between processes. When a new launcher instance starts, it first attempts to connect to an existing IPC server. If the connection succeeds, the new process sends a toggle message to the existing launcher instance and exits immediately. If no server is found, the process becomes the primary instance and starts the IPC server.

This design prevents duplicate launcher windows and enables launcher toggling behavior similar to global hotkey systems. If the launcher is hidden, receiving a toggle signal makes it visible and focused. If the launcher is already visible, the signal hides the window.

Qt's signal and slot mechanism is heavily used throughout the system. Signals are emitted whenever events occur, such as text input changes or item activation. Slots respond to these signals and trigger the appropriate application logic. For example, when the user types into the search box, the textChanged() signal is emitted, which triggers the handleQueryChanged() slot to start a new search operation.



**Search System**

The search system is designed around a lightweight in-memory pipeline optimized for fast response times. Once indexing is complete, all search operations are performed entirely in memory without additional filesystem access or disk I/O.

When the user enters a query, the search engine first normalizes the input by converting it to lowercase and preparing it for case-insensitive matching. The query is then split into tokens, allowing multi-word searches to be evaluated independently.

Each indexed item is evaluated against the query tokens using a simple but efficient matching strategy. Exact matches receive the highest priority, prefix matches using startsWith() receive strong relevance scores, and partial matches using contains() receive lower scores. The system intentionally avoids typo correction and fuzzy matching to maintain predictable performance and implementation simplicity.

The scoring system calculates a relevance value based on match type, token coverage, and positional strength. Prefix matches are prioritized over partial matches, and items matching more tokens receive higher scores. The final result list is sorted in descending order of relevance, and only the top-ranked entries are displayed.

Project Alfred also includes adaptive ranking behavior. Each time a user launches an item, the selection frequency for that item increases. This frequency value contributes additional score weight during future searches, allowing frequently used applications and files to naturally appear higher in the result list over time.

When a result is selected, the launcher evaluates the item type and executes the appropriate system action. Applications are launched using their executable or .desktop metadata, while files and directories are opened using the operating system's default handlers.



**File System Indexing**

The indexing system scans the operating system and builds the searchable dataset used by the launcher. This process follows a controlled single-pass indexing pipeline consisting of root scanning, recursive traversal, filtering, parsing, deduplication, and in-memory storage.

The indexer begins scanning from predefined locations such as the Home directory, Desktop, Documents, and Downloads folders. Restricting indexing to these areas ensures that only relevant user-accessible content is included in the search dataset.

Recursive traversal is carefully controlled through maximum depth limits and path deduplication mechanisms. The visitedPaths structure prevents infinite recursion and avoids duplicate indexing caused by symbolic links or repeated paths.

The indexing system filters irrelevant or expensive content during traversal. Hidden directories, system folders, and development-related directories such as .git and node_modules are skipped entirely. Only allowed file types are indexed, ensuring that the final dataset remains lightweight and relevant.

Linux applications are indexed separately through .desktop file parsing. The system extracts application names, execution commands, and icon paths from desktop entry metadata while ignoring invalid or hidden entries. Deduplication rules prevent duplicate application records from appearing in the dataset.

All indexed content is converted into a lightweight IndexedItem structure containing only the information required during runtime search operations. This minimal representation reduces memory overhead and allows the search engine to process large numbers of items efficiently.



**Performance Design**

Project Alfred is built around the philosophy of scanning once and searching many times. All expensive filesystem operations are performed during startup indexing, while all runtime searches operate entirely in memory.

The architecture assumes a dataset size ranging from hundreds to a few thousand searchable entries. This controlled scale allows the launcher to use simple linear search and lightweight scoring algorithms without introducing noticeable performance issues.

The indexing process is optimized through controlled traversal depth, path deduplication, and aggressive filtering of unnecessary content. The search system itself relies only on lightweight string operations and integer scoring, avoiding computationally expensive algorithms or advanced ranking systems.

The application intentionally prioritizes simplicity and predictability over feature complexity. By avoiding fuzzy search engines, live filesystem monitoring, and heavyweight indexing frameworks, Project Alfred maintains extremely fast response times and low system resource usage.



**Technologies**

Project Alfred is developed using C++ and the Qt Framework. The application relies on Qt's event-driven architecture, signal-slot system, filesystem utilities, and widget framework to provide a lightweight desktop launcher experience on Linux systems.

The project also uses Linux filesystem APIs and inter-process communication mechanisms to support indexing, application launching, and single-instance behavior.

**Future Improvements**

Future versions of Project Alfred may include additional functionality such as fuzzy search support, real-time filesystem monitoring, plugin architecture, clipboard history integration, calculator functionality, command execution mode, theme customization, and improved Wayland compatibility.

The current implementation intentionally focuses on building a fast, lightweight, and maintainable foundation before introducing more advanced features.

**Author**
Lucas Fernandes
