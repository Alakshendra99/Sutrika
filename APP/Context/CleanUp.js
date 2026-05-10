const CleanupHandlers = new Map();

function RegisterCleanup(Name, Handler) {
  CleanupHandlers.set(Name, Handler);
}

function UnregisterCleanup(Name) {
  CleanupHandlers.delete(Name);
}

async function CleanupApplication() {
  console.log("Running CleanUp ...");
  for (const [Name, Handler] of CleanupHandlers) {
    try {
      console.log(`Cleaning: ${Name}`);
      await Handler();

    }
    catch (Error) {
      console.error(`Cleanup Failed: ${Name}`, Error);
    }
  }
  CleanupHandlers.clear();
  console.log("CleanUp Completed");
}

module.exports = {
  RegisterCleanup,
  UnregisterCleanup,
  CleanupApplication
};