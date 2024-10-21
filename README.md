# Menu-Helper

A program to manage a database of recipes and help you to pick out meals based
on filters of ingredients and tags.

## Usage

Ensure the `XDG_DATA_HOME` variable is set (e.g. to `$HOME/.local/share`) and
that you have installed the SQLite3 library.

Upon first execution of any command, the program will automatically create the
database.

### Adding New Recipes

The first thing you're probably going to want to do is to add a new recipe to
your database. If this database hasn't been created already then the program
will do it automatically. This is done via the `add` subcommand, which will
query you about the different attributes you want for your recipe, looking
something like the following:

```console
$ menu-helper add
Name: Linguine Scampi
Description: A lemony Italian pasta dish.
Ingredients (comma separated): linguine,shrimp,garlic,parsley,lemon
Tags (comma separated): italian,lunch
Creating database in /home/nicolas/.local/share/menu-helper/recipes.db
```

This will have created your recipe within the database. That last line there is
merely informative, telling you that the database did not exist and it is not
being created; if you had a database already and it isn't being found, ensure
that your `XDG_DATA_HOME` environment variable is properly set.

### Querying Recipes

#### Filtering

Once a recipe or two have been added to your database you may now query them
filtering based on ingredients and tags. This is done via the `list` command,
which takes two kinds of arguments, both optional:

- `-i <list>`: Comma-separated list of the ingredients to look for.
- `-t <list>`: Comma-separated list of the tags to look for.

If neither is specified then all recipes will be listed with their respective
ID, name, and description:

```console
$ menu-helper list
1  |  Linguine Scampi  |  A lemony Italian pasta dish.
2  |  Garlic Soup  |  A simple monastic soup for cold winters.
```

However, when one of these arguments is used it filters recipes to only show
those which include __all__ the ingredients and tags specified:

```console
$ menu-helper list -i linguine
1  |  Linguine Scampi  |  A lemony Italian pasta dish.
```

#### Recipe Information

The IDs shown in the queries above now become useful for the rest of
Menu-Helper functionality. If you wish to query all stored information about a
given recipe, this is where you can use the `info` subcommand with the relevant
ID:

```console
$ menu-helper info 2
Name: Garlic Soup
Description: A simple monastic soup for cold winters.
ID: 2

Ingredients:
        - garlic
        - bread
        - egg

Tags:
        - soup
        - dinner
        - simple

```

### Removing Recipes

If you end up desiring to remove a recipe for whatever reason, you can do so by
using the `del` subcommand with the recipe's corresponding ID:

```console
$ menu-helper del 2
$ menu-helper list
1  |  Linguine Scampi  |  A lemony Italian pasta dish.
```

### Modifying Recipes

#### Name & Description

To correct or otherwise modify the name or description of your recipe, you can
use the `edit-name` and `edit-description` subcommands. These will prompt you
for the new name or description respectively and overwrite what was previously
stored in the database:

```console
$ menu-helper edit-name 1
New name: Lenguine agli Scampi
$ menu-helper edit-description 1
New description: A zesty Italian pasta dish.
$ menu-helper list
1  |  Linguine agli Scampi  | A zesty Italian pasta dish.
```

#### Ingredients/Tags

If there are ingredients/tags which you forgot to add to a recipe, or that you
added erringly, you can correct this with the following commands:

- `add-ingr <id> <list>`: Add list of comma-separated ingredients `list` to
  recipe with ID `id`.
- `rm-ingr <id> <list>`: Remove list of comma-separated ingredients `list` from
  recipe with ID `id`.
- `add-tag <id> <list>`: Add list of comma-separated tags `list` to recipe with
  ID `id`.
- `rm-tag <id> <list>`: Remove list of comma-separated tags `list` from recipe
  with ID `id`.

For example, we forgot to add the useful tag to our first recipe (Linguine
Scampi) that it is a pasta dish. We can do this with the following command:

```console
$ menu-helper add-tag 1 pasta
$ menu-helper info 1
Name: Linguine agli Scampi
Description: A zesty Italian pasta dish.
ID: 1

Ingredients:
        - linguine
        - shrimp
        - garlic
        - parsley
        - lemon

Tags:
        - italian
        - lunch
        - pasta

```

## Building

To build the program you will require the following dependencies:

- A C++ compiler compatible with C++20 (preferably GCC).
- SQLite3 C/C++ library
- Make

Once installed, compile the project with the `make` command. To install simply
run the `make install` command, optionally appending `PREFIX=...` to change the
default directory of installation (i.e. `/usr/local/...`).

## Contributing

If you find any issues, feel free to report them on GitHub or send me an E-Mail
(see my website/profile for the address). I will add these issues to my personal
Gitea page and (unless specified otherwise) mention you as the person who found
the issue.

For patches/pull requests, if you open a PR on GitHub I will likely not merge
directly but instead apply the patches locally (via Git patches, conserving
authorship), push them to my Gitea repository, which will finally be mirrored to
GitHub. However, you can save me a bit of work by just sending me the Git
patches directly (via E-Mail).

If you're looking for a way to contribute, consider having a look at my [To-Do
list](TODO.md) for the project.

## License

This program is licensed under the terms & conditions of the GNU General Public
License version 3 or greater. See the [LICENSE](LICENSE) file for more
information.
